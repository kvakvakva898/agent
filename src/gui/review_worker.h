#pragma once 

#include <QObject>
#include <QString>

#include <atomic>
#include <filesystem>
#include <vector>

#include "core/review_issue.h"
#include "core/agent_config.h"
#include "core/code_review_service.h"

class ReviewWorker : public QObject
{
	Q_OBJECT

public:	
	ReviewWorker(
		CodeReviewService& review_agent,
		const QString& project_path);

public slots:
	void process();
	void cancel();

signals:
	void finished(
		const std::vector<ReviewIssue>& issues);

	void cancelled();

	void error(
		const QString& message);

	void progress(
		const QString& message);

private:
	CodeReviewService& review_agent_;
	QString project_path_;

	std::atomic_bool cancel_requested_{false};
};