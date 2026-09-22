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
		emit progress(
			"Starting project analysis...");

		CodeReviewAgent agent(
			config_);

		emit progress(
			"Analyzing project...");

		const auto issues =
			agent.analyze(
				std::filesystem::path(
					project_path_.toStdWString()));

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