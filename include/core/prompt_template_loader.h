#pragma once

#include <filesystem>
#include <string>

class PromptTemplateLoader
{
public:
	std::string load(
		const std::filesystem::path& path
	) const;
};