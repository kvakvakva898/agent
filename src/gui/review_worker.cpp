#include "gui/review_worker.h"

#include "core/code_review_service.h"

#include <exception>
#include <filesystem>

ReviewWorker::ReviewWorker(
	std::shared_ptr<CodeReviewService> review_agent,
	const QString& project_path):
	review_agent_(std::move(review_agent)),
	project_path_(project_path)
{
}

void ReviewWorker::process()
{
	try
	{
		emit progress("Starting project analysis...");

		const auto issues =
			review_agent_->analyze(
				std::filesystem::path(
					project_path_.toStdWString()),
				[this](const std::string& message)
				{
					emit progress(
						QString::fromStdString(message)
					);
				},
				&cancel_requested_);

		if (cancel_requested_.load(
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
	catch (const RequestCancelled&)
	{
		emit cancelled();
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
	cancel_requested_.store(
		true,
		std::memory_order_relaxed);
}