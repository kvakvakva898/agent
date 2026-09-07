#pragma once

#include <filesystem>
#include <vector>

#include "project_loader.h"
#include "code_chunker.h"
#include "prompt_builder.h"
#include "llm_client.h"
#include "review_result_parser.h"
#include "prompt_template_loader.h"
#include "program_options.h"

class CodeReviewAgent
{
public:

	explicit CodeReviewAgent(
		const ProgramOptions& options,
		const std::filesystem::path& prompt_template_path
	);

	std::vector<ReviewIssue> analyze(
		const std::filesystem::path& project_path
	);

private:



	ProjectLoader project_loader_;

	CodeChunker chunker_;

	PromptBuilder prompt_builder_;

	LLMClient llm_client_;

	ReviewResultParser result_parser_;
};