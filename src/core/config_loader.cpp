#include "core/config_loader.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

AgentConfig ConfigLoader::load(
	const std::filesystem::path& path) const
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		throw std::runtime_error(
			"Cannot open config file: " + path.string());
	}

	json data;

	try
	{
		file >> data;
	}
	catch (const json::exception& e)
	{
		throw std::runtime_error(
			std::string("Invalid config json: ") + e.what());
	}

	AgentConfig config;

	try
	{
		config.endpoint =
			data.at("endpoint").get<std::string>();

		config.model =
			data.at("model").get<std::string>();

		config.prompt_template_path =
			data.at("prompt_template_path").get<std::string>();
	}
	catch (const json::exception& e)
	{
		throw std::runtime_error(
			std::string("Invalid config fields: ") + e.what());
	}

	if (config.endpoint.empty())
		throw std::runtime_error("LLM endpoint cannot be empty.");

	if (config.model.empty())
		throw std::runtime_error("LLM model cannot be empty.");

	if (config.prompt_template_path.empty())
		throw std::runtime_error("Prompt template path cannot be empty.");

	return config;
}

void ConfigLoader::save(
	const AgentConfig& config,
	const std::filesystem::path& path) const
{
	json data =
	{
		{"endpoint", config.endpoint},
		{"model", config.model},
		{"prompt_template_path", config.prompt_template_path.string()}
	};

	std::ofstream file(path);

	if (!file.is_open())
	{
		throw std::runtime_error(
			"Cannot write config file: " + path.string());
	}

	file << data.dump(4);

	if (!file)
	{
		throw std::runtime_error(
			"Failed to write config gile.");
	}
}