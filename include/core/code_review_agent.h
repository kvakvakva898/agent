#pragma once

#include <filesystem>
#include <vector>

#include "core/project_loader.h"
#include "core/code_chunker.h"
#include "core/prompt_builder.h"
#include "core/llm_client.h"
#include "core/review_result_parser.h"
#include "core/prompt_template_loader.h"
#include "agent_config.h"

class CodeReviewAgent
{
public:

	explicit CodeReviewAgent(
		const AgentConfig& config);

	std::vector<ReviewIssue> analyze(
		const std::filesystem::path& project_path);

private:



	ProjectLoader project_loader_;

	CodeChunker chunker_;

	PromptBuilder prompt_builder_;

	LLMClient llm_client_;

	ReviewResultParser result_parser_;
};