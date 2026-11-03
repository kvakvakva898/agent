#pragma once

#include <QMainWindow>

#include <filesystem>
#include <vector>

#include "core/review_issue.h"
#include "core/agent_config.h"
#include "core/code_review_service.h"

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
		CodeReviewService& review_agent,
		QWidget* parent = nullptr);

private slots:
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
	void displayIssues();

	void displayIssueDetails(
		const ReviewIssue& issue
	);

private:
	CodeReviewService& review_agent_;

	std::vector<ReviewIssue> issues_;

	QLineEdit* project_path_edit_;
	QPushButton* browse_button_;
	QPushButton* analyze_button_;
	QPushButton* cancel_button_;

	QTableWidget* issues_table_;
	QTextEdit* details_edit_;
	QProgressBar* progress_bar_;

	QLabel* status_label_;

	QThread* review_thread_ = nullptr;
	ReviewWorker* review_worker_ = nullptr;
};