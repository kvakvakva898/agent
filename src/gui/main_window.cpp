#include "main_window.h"

#include "core/code_review_agent.h"
#include "core/agent_config.h"
#include "review_worker.h"

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

#include <filesystem>
#include <sstream>
#include <thread>

MainWindow::MainWindow(
	AgentConfig config,
	QWidget* parent):
	QMainWindow(parent),
	config_(std::move(config))
{
	setWindowTitle("Code Review Agent");
	resize(1100, 700);

	auto* central_widget =
		new QWidget(
			this);

	setCentralWidget(
		central_widget);

	auto* main_layout =
		new QVBoxLayout(
			central_widget);

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

	project_layout->addWidget(
		project_path_edit_);

	project_layout->addWidget(
		browse_button_);

	project_layout->addWidget(
		analyze_button_);

	main_layout->addLayout(
		project_layout);

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

	status_label_ =
		new QLabel("Ready", this);

	main_layout->addWidget(
		status_label_);

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

	analyze_button_->setEnabled(false);

	browse_button_->setEnabled(false);

	status_label_->setText(
		"Analyzing project...");

	review_thread_ =
		new QThread(this);

	review_worker_ =
		new ReviewWorker(
			config_,
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
		&ReviewWorker::finished,
		review_thread_,
		&QThread::quit);

	connect(
		review_worker_,
		&ReviewWorker::error,
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

void MainWindow::onAnalysisFinished(
	const std::vector<ReviewIssue>& issues)
{
	issues_ = issues;

	displayIssues();

	status_label_->setText(
		QString("Analysis complete. Found %1 issues")
			.arg(issues.size()));
}

void MainWindow::onAnalysisError(
	const QString& message)
{
	status_label_->setText(
		"Analysis failed.");

	QMessageBox::critical(
		this,
		"Analysis failed",
		message);
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