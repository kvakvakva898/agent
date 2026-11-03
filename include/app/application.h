#pragma once

#include <string>

#include "core/agent_config.h"

#include "core/project_loader.h"
#include "core/code_chunker.h"
#include "core/prompt_builder.h"
#include "core/llm_client.h"
#include "core/review_result_parser.h"
#include "core/prompt_template_loader.h"
#include "core/code_review_service.h"

class Application
{
public:
	explicit Application(
		const AgentConfig& config);

	CodeReviewService& reviewAgent();

private:

	ProjectLoader project_loader_;
	CodeChunker chunker_;
	PromptBuilder prompt_builder_;
	LLMClient llm_client_;
	ReviewResultParser result_parser_;

	CodeReviewService review_agent_;
};