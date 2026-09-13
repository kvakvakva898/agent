#include "core/prompt_template_loader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

std::string PromptTemplateLoader::load(
	const std::filesystem::path& path
) const
{
	std::ifstream file(path);

	if (!file)
	{
		throw std::runtime_error(
			"Failed to open prompt template: "
			+ path.string()
		);
	}

	std::stringstream buff;

	buff << file.rdbuf();

	return buff.str();
}