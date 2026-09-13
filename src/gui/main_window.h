#pragma once

#include <QMainWindow>

#include <filesystem>
#include <vector>

#include "core/review_issue.h"

class QLineEdit;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QLabel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(
		const std::string& endpoint,
		const std::string& model,
		const std::filesystem::path& prompt_template_path,
		QWidget* parent = nullptr);

private:
	void browseProject();
	void analyzeProject();
	void showSelectedIssue();

private:
	void displayIssues();

	void displayIssueDetails(
		const ReviewIssue& issue
	);

private:
	std::string endpoint_;
	std::string model_;
	std::filesystem::path prompt_template_path_;

	std::vector<ReviewIssue> issues_;

	QLineEdit* project_path_edit_;
	QPushButton* browse_button_;
	QPushButton* analyze_button_;

	QTableWidget* issues_table_;
	QTextEdit* details_edit_;

	QLabel* status_label_;
};