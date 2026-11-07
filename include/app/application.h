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

	CodeReviewService& reviewService();

	const AgentConfig& config() const;

	void applyConfig(const AgentConfig& config);

	void saveConfig() const;

private:
	struct Runtime;

	std::unique_ptr<Runtime> runtime_;

	AgentConfig config_;
	std::filesystem::path config_path_;
};