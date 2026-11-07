#include "main_window.h"

#include "core/code_review_service.h"
#include "core/agent_config.h"
#include "review_worker.h"

#include <QTabWidget>
#include <QFormLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QProgressBar>

#include <filesystem>
#include <sstream>
#include <thread>

MainWindow::MainWindow(
	Application& application,
	QWidget* parent):
	QMainWindow(parent),
	application_(application)
{
	setWindowTitle("Code Review Agent");
	resize(600, 500);

	tabs_ =
		new QTabWidget(
			this);

	setCentralWidget(
		tabs_);

	auto* review_tab =
		new QWidget(this);

	tabs_->addTab(review_tab, "Code review");

	createSettingsTab();
	loadSettingsToUi();

		auto* main_layout =
			new QVBoxLayout(
				review_tab);

		auto* project_layout =
			new QHBoxLayout();

	project_path_edit_ =
		new QLineEdit(
			this);

	project_path_edit_->setPlaceholderText(
		"Select C++ project directory");

	browse_button_ =
		new QPushButton(
			"Browse...",
			this);

	analyze_button_ =
		new QPushButton(
			"Analyze project",
			this);

	cancel_button_ =
		new QPushButton(
			"Cancel",
			this);

	cancel_button_->setEnabled(false);

	auto* progress_layout_ = new QBoxLayout(QBoxLayout::TopToBottom);

	status_label_ =
		new QLabel("Ready", this);

	progress_bar_ =
		new QProgressBar(this);

	progress_bar_->setRange(0, 0);
	progress_bar_->setVisible(false);

	progress_layout_->addWidget(
		progress_bar_);

	progress_layout_->addWidget(
		status_label_);

	project_layout->addWidget(
		project_path_edit_);

	project_layout->addWidget(
		browse_button_);

	project_layout->addWidget(
		analyze_button_);

	project_layout->addWidget(
		cancel_button_);

	main_layout->addLayout(
		project_layout);

	main_layout->addLayout(
		progress_layout_);

	issues_table_ =
		new QTableWidget(
			this);

	issues_table_->setColumnCount(5);

	issues_table_->setHorizontalHeaderLabels({
		"Severity",
		"Category",
		"File",
		"Line",
		"Confidence"
		});

	issues_table_->horizontalHeader()
		->setStretchLastSection(true);

	issues_table_->horizontalHeader()
		->setSectionResizeMode(
			2,
			QHeaderView::Stretch);

	issues_table_->setSelectionBehavior(
		QAbstractItemView::SelectRows);

	issues_table_->setSelectionMode(
		QAbstractItemView::SingleSelection);

	issues_table_->setEditTriggers(
		QAbstractItemView::NoEditTriggers);

	main_layout->addWidget(
		issues_table_);

	details_edit_ =
		new QTextEdit(this);

	details_edit_->setReadOnly(true);

	details_edit_->setPlaceholderText(
		"Select an issue to see details...");

	main_layout->addWidget(
		details_edit_);

	connect(
		browse_button_,
		&QPushButton::clicked,
		this,
		&MainWindow::browseProject);

	connect(
		analyze_button_,
		&QPushButton::clicked,
		this,
		&MainWindow::analyzeProject);

	connect(
		cancel_button_,
		&QPushButton::clicked,
		this,
		&MainWindow::cancelAnalysis);

	connect(
		issues_table_,
		&QTableWidget::itemSelectionChanged,
		this,
		&MainWindow::showSelectedIssue);
}

void MainWindow::browseProject()
{
	const QString directory =
		QFileDialog::getExistingDirectory(
			this,
			"Select C++ project");

	if (!directory.isEmpty())
	{
		project_path_edit_->setText(
			directory);
	}
}

void MainWindow::analyzeProject()
{
	if (review_thread_ && review_thread_->isRunning())
		return;

	const QString path =
		project_path_edit_->text().trimmed();

	if (path.isEmpty())
	{
		QMessageBox::warning(
			this,
			"No project",
			"Please select a project directory.");

		return;
	}

	progress_bar_->setVisible(true);

	cancel_button_->setEnabled(true);

	analyze_button_->setEnabled(false);

	browse_button_->setEnabled(false);

	status_label_->setText(
		"Analyzing project...");

	review_thread_ =
		new QThread(this);

	review_worker_ =
		new ReviewWorker(
			application_.reviewService(),
			path);

	review_worker_->moveToThread(
		review_thread_);

	connect(
		review_thread_,
		&QThread::started,
		review_worker_,
		&ReviewWorker::process);

	connect(
		review_worker_,
		&ReviewWorker::progress,
		this,
		[this](const QString& message)
		{
			status_label_->setText(message);
		});

	connect(
		review_worker_,
		&ReviewWorker::finished,
		this,
		&MainWindow::onAnalysisFinished);

	connect(
		review_worker_,
		&ReviewWorker::error,
		this,
		&MainWindow::onAnalysisError);

	connect(
		review_worker_,
		&ReviewWorker::cancelled,
		this,
		&MainWindow::onAnalysisCancelled);

	connect(
		review_worker_,
		&ReviewWorker::finished,
		review_thread_,
		&QThread::quit);

	connect(
		review_worker_,
		&ReviewWorker::error,
		review_thread_,
		&QThread::quit);

	connect(
		review_worker_,
		&ReviewWorker::cancelled,
		review_thread_,
		&QThread::quit);

	connect(
		review_thread_,
		&QThread::finished,
		review_worker_,
		&QObject::deleteLater);

	connect(
		review_thread_,
		&QThread::finished,
		review_thread_,
		&QObject::deleteLater);

	connect(
		review_thread_,
		&QThread::finished,
		this,
		[this]()
		{
			review_thread_ = nullptr;
			review_worker_ = nullptr;

			analyze_button_->setEnabled(true);
			browse_button_->setEnabled(true);
		});

	review_thread_->start();
}

void MainWindow::createSettingsTab()
{
	auto* settings_tab = new QWidget(this);
	auto* layout = new QVBoxLayout(settings_tab);

	auto* form = new QFormLayout();

	endpoint_edit_ = new QLineEdit(settings_tab);
	model_edit_ = new QLineEdit(settings_tab);
	prompt_path_edit_ = new QLineEdit(settings_tab);

	browse_prompt_button_ =
		new QPushButton("Browse...", settings_tab);

	auto* prompt_layout = new QHBoxLayout();

	prompt_layout->addWidget(prompt_path_edit_);
	prompt_layout->addWidget(browse_prompt_button_);

	form->addRow("LLM endpoint:", endpoint_edit_);
	form->addRow("Model:", model_edit_);
	form->addRow("Prompt template:", prompt_layout);

	save_settings_button_ =
		new QPushButton("Save settings", settings_tab);

	layout->addLayout(form);
	layout->addWidget(save_settings_button_);
	layout->addStretch();

	tabs_->addTab(settings_tab, "Settings");

	connect(
		browse_prompt_button_,
		&QPushButton::clicked,
		this,
		&MainWindow::browsePromptTemplate);

	connect(
		save_settings_button_,
		&QPushButton::clicked,
		this,
		&MainWindow::saveSettings);
}

void MainWindow::loadSettingsToUi()
{
	const auto& config = application_.config();

	endpoint_edit_->setText(
		QString::fromStdString(config.endpoint));

	model_edit_->setText(
		QString::fromStdString(config.model));

	prompt_path_edit_->setText(
		QString::fromStdString(
			config.prompt_template_path.string()));
}

void MainWindow::browsePromptTemplate()
{
	const QString path =
		QFileDialog::getOpenFileName(
			this,
			"Select prompt template",
			{},
			"Text files (*.txt);;All files (*.*)");

	if (!path.isEmpty())
	{
		prompt_path_edit_->setText(path);
	}
}

void MainWindow::saveSettings()
{
	AgentConfig config;

	config.endpoint =
		endpoint_edit_->text().trimmed().toStdString();

	config.model =
		model_edit_->text().trimmed().toStdString();

	config.prompt_template_path =
		prompt_path_edit_->text().trimmed().toStdString();

	if (config.endpoint.empty() ||
		config.model.empty() ||
		config.prompt_template_path.empty())
	{
		QMessageBox::warning(
			this,
			"Invalid settings",
			"All settings fields must be filled.");

		return;
	}

	if (!std::filesystem::exists(
		config.prompt_template_path) ||
		!std::filesystem::is_regular_file(
			config.prompt_template_path))
	{
		QMessageBox::warning(
			this,
			"Invalid prompt template",
			"Prompt template file does not exist.");

		return;
	}

	try
	{
		application_.applyConfig(config);
		application_.saveConfig();

		QMessageBox::information(
			this,
			"Settings",
			"Settings saved successfully.");
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(
			this,
			"Settings error",
			QString::fromUtf8(e.what()));
	}
}

void MainWindow::onAnalysisFinished(
	const std::vector<ReviewIssue>& issues)
{
	progress_bar_->setVisible(false);

	issues_ = issues;

	displayIssues();

	status_label_->setText(
		QString("Analysis complete. Found %1 issues")
			.arg(issues.size()));
}

void MainWindow::onAnalysisError(
	const QString& message)
{
	progress_bar_->setVisible(false);

	status_label_->setText(
		"Analysis failed.");

	QMessageBox::critical(
		this,
		"Analysis failed",
		message);
}

void MainWindow::onAnalysisCancelled()
{
	progress_bar_->setVisible(false);

	status_label_->setText(
		"Analysis cancelled.");

	analyze_button_->setEnabled(true);
	browse_button_->setEnabled(true);
}

void MainWindow::cancelAnalysis()
{
	if (!review_worker_)
		return;

	cancel_button_->setEnabled(false);

	review_worker_->cancel();

}

void MainWindow::displayIssues()
{
	issues_table_->clearContents();

	issues_table_->setRowCount(
		static_cast<int>(issues_.size()));

	for (int row = 0;
		 row < static_cast<int>(issues_.size());
		 ++row)
	{
		const auto& issue =
			issues_[row];

		issues_table_->setItem(
			row,
			0,
			new QTableWidgetItem(
				QString::fromStdString(
					issue.severity)));
		issues_table_->setItem(
			row,
			1,
			new QTableWidgetItem(
				QString::fromStdString(
					issue.category)));

		issues_table_->setItem(
			row,
			2,
			new QTableWidgetItem(
				QString::fromStdString(
					issue.source_file.string())));

		issues_table_->setItem(
			row,
			3,
			new QTableWidgetItem(
				QString::number(
					issue.line)));

		issues_table_->setItem(
			row,
			4,
			new QTableWidgetItem(
				QString::number(
					issue.confidence,
					'f',
					2)));
	}

	if (!issues_.empty())
	{
		issues_table_->selectRow(0);

		displayIssueDetails(
			issues_[0]);
	}
}

void MainWindow::showSelectedIssue()
{
	const int row =
		issues_table_->currentRow();

	if (row < 0 ||
		row >= static_cast<int>(issues_.size()))
	{
		details_edit_->clear();
		
		return;
	}

	displayIssueDetails(
		issues_[row]);
}

void MainWindow::displayIssueDetails(
	const ReviewIssue& issue)
{
	std::ostringstream text;

	text
		<< "Severity: "
		<< issue.severity
		<< "\n";

	text
		<< "Category: "
		<< issue.category
		<< "\n";

	text
		<< "File: "
		<< issue.source_file.string()
		<< "\n";

	text
		<< "Line: "
		<< issue.line
		<< "\n";

	text
		<< "Confidence: "
		<< issue.confidence
		<< "\n";

	text
		<< "Description: "
		<< issue.description
		<< "\n";

	text
		<< "Recommendation: "
		<< issue.recommendation
		<< "\n\n";

	details_edit_->setPlainText(
		QString::fromStdString(
			text.str()));
}