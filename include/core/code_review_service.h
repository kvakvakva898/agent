#pragma once

#include <filesystem>
#include <vector>
#include <functional>
#include <string>
#include <atomic>

#include "core/project_loader.h"
#include "core/code_chunker.h"
#include "core/prompt_builder.h"
#include "core/llm_client.h"
#include "core/review_result_parser.h"

class CodeReviewService
{
public:

	explicit CodeReviewService(
		ProjectLoader& project_loader,
		CodeChunker& chunker,
		PromptBuilder& prompt_builder,
		LLMClient& llm_client,
		ReviewResultParser& result_parser);

	std::vector<ReviewIssue> analyze(
		const std::filesystem::path& project_path,
		std::function<void(const std::string&)> progress = {},
		const std::atomic_bool* cancel_requested = nullptr);

private:

	ProjectLoader& project_loader_;
	CodeChunker& chunker_;
	PromptBuilder& prompt_builder_;
	LLMClient& llm_client_;
	ReviewResultParser& result_parser_;
};