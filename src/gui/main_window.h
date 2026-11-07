#pragma once

#include <QMainWindow>

#include <filesystem>
#include <vector>

#include "core/review_issue.h"
#include "core/agent_config.h"
#include "app/application.h"

class QTabWidget;
class QFormLayout;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QLabel;
class QThread;
class ReviewWorker;
class QProgressBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(
		Application& application,
		QWidget* parent = nullptr);

private slots:
	void browsePromptTemplate();
	void saveSettings();
	void browseProject();
	void analyzeProject();
	void showSelectedIssue();
	void cancelAnalysis();

	void onAnalysisFinished(
		const std::vector <ReviewIssue>& issues);

	void onAnalysisError(
		const QString& message);

	void onAnalysisCancelled();

private:
	void createSettingsTab();
	void loadSettingsToUi();

	void displayIssues();

	void displayIssueDetails(
		const ReviewIssue& issue
	);

private:
	Application& application_;

	std::vector<ReviewIssue> issues_;

	QLineEdit* endpoint_edit_;
	QLineEdit* model_edit_;
	QLineEdit* prompt_path_edit_;

	QLineEdit* project_path_edit_;
	QPushButton* browse_button_;
	QPushButton* analyze_button_;
	QPushButton* cancel_button_;
	QPushButton* browse_prompt_button_;
	QPushButton* save_settings_button_;

	QTabWidget* tabs_;
	QTableWidget* issues_table_;
	QTextEdit* details_edit_;
	QProgressBar* progress_bar_;

	QLabel* status_label_;

	QThread* review_thread_ = nullptr;
	ReviewWorker* review_worker_ = nullptr;
};