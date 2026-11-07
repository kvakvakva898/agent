#pragma once

#include "core/agent_config.h"

#include <filesystem>

class ConfigLoader
{
public:
	AgentConfig load(
		const std::filesystem::path& path) const;

	void save(
		const AgentConfig& config,
		const std::filesystem::path& path) const;
};