#include "review_worker.h"

#include "core/code_review_agent.h"

#include <exception>
#include <filesystem>

ReviewWorker::ReviewWorker(
	const AgentConfig& config,
	const QString& project_path):
	config_(config),
	project_path_(project_path)
{
}

void ReviewWorker::process()
{
	try
	{
		emit progress("Starting project analysis...");

		CodeReviewAgent agent(
			config_);

		const auto issues =
			agent.analyze(
				std::filesystem::path(
					project_path_.toStdWString()),
				[this](const std::string& message)
				{
					emit progress(
						QString::fromStdString(message)
					);
				},
				&calcel_requested_);

		if (calcel_requested_.load(
			std::memory_order_relaxed))
		{
			emit cancelled();
			return;
		}

		emit progress(
			"Analysis finished.");

		emit finished(
			issues);
	}
	catch (const std::exception& e)
	{
		emit error(
			QString::fromUtf8(e.what()));
	}
	catch (...)
	{
		emit error(
			"Unknown error occurred.");
	}
}

void ReviewWorker::cancel()
{
	calcel_requested_.store(
		true,
		std::memory_order_relaxed);

	emit progress(
		"Stopping analysis...");
}