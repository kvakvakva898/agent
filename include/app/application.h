#pragma once

#include <filesystem>
#include <memory>

#include "core/agent_config.h"

class CodeReviewService;

class Application
{
public:
	Application(
		const AgentConfig& config,
		const std::filesystem::path& config_path);

	~Application();

	std::shared_ptr<CodeReviewService> reviewServiceHandle() const;

	const AgentConfig& config() const;

	void applyConfig(const AgentConfig& config);

	void saveConfig() const;

private:
	struct Runtime;

	std::shared_ptr<Runtime> runtime_;

	AgentConfig config_;
	std::filesystem::path config_path_;
};