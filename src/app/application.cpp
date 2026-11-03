#include "app/application.h"
#include "core/code_review_service.h"

Application::Application(
	const AgentConfig& config)
	: prompt_builder_(
		PromptTemplateLoader{}.load(
			config.prompt_template_path)),
	  llm_client_(
		config.endpoint,
		config.model),
	  review_agent_(
		project_loader_,
		chunker_,
		prompt_builder_,
		llm_client_,
		result_parser_)
{
}

CodeReviewService& Application::reviewAgent()
{
	return review_agent_;
}