#include "app/application.h"

#include "core/code_review_service.h"
#include "core/project_loader.h"
#include "core/code_chunker.h"
#include "core/prompt_builder.h"
#include "core/prompt_template_loader.h"
#include "core/llm_client.h"
#include "core/review_result_parser.h"
#include "core/config_loader.h"

struct Application::Runtime
{
	ProjectLoader project_loader;
	CodeChunker chunker;

	PromptBuilder prompt_builder;

	LLMClient llm_client;
	ReviewResultParser result_parser;

	CodeReviewService review_service;

	explicit Runtime(const AgentConfig& config)
		: prompt_builder(
			PromptTemplateLoader{}.load(
				config.prompt_template_path)),
		  llm_client(
			  config.endpoint,
			  config.model),
		  review_service(
			  project_loader,
			  chunker,
			  prompt_builder,
			  llm_client,
			  result_parser)
	{
	}
};

Application::Application(
	const AgentConfig& config,
	const std::filesystem::path& config_path)
	: config_(config),
	  config_path_(config_path)
{
	runtime_ = std::make_shared<Runtime>(config_);
}

Application::~Application() = default;

std::shared_ptr<CodeReviewService>
Application::reviewServiceHandle() const
{
	return std::shared_ptr<CodeReviewService>(
		runtime_,
		&runtime_->review_service);
}

const AgentConfig& Application::config() const
{
	return config_;
}

void Application::applyConfig(const AgentConfig& config)
{
	auto new_runtime =
		std::make_shared<Runtime>(config);

	runtime_ = std::move(new_runtime);
	config_ = config;
}

void Application::saveConfig() const
{
	ConfigLoader loader;
	loader.save(config_, config_path_);
}