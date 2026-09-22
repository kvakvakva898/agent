#pragma once 

#include <QObject>
#include <QString>

#include <filesystem>
#include <vector>

#include <core/review_issue.h>
#include <core/agent_config.h>

class ReviewWorker : public QObject
{
	Q_OBJECT

public:	
	ReviewWorker(
		const AgentConfig& config,
		const QString& project_path);

public slots:
	void process();

signals:
	void finished(
		const std::vector<ReviewIssue>& issues);

	void error(
		const QString& message);

	void progress(
		const QString& message);

private:
	AgentConfig config_;
	QString project_path_;
};