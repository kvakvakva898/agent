#pragma once

#include <string>
#include <filesystem>

struct AgentConfig
{
	std::string endpoint;
	std::string model;
	std::filesystem::path prompt_template_path;
};