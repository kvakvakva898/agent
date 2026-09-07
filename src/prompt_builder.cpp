#include "prompt_builder.h"

namespace
{
	void replace(
		std::string& text,
		const std::string& placeholder,
		const std::string& value)
	{
		size_t position = 0;

		while (
			(position = text.find(
				placeholder,
				position
			)) != std::string::npos)
		{
			text.replace(
				position,
				placeholder.length(),
				value
			);

			position += value.length();
		}
	}
}

PromptBuilder::PromptBuilder(
	std::string prompt_template)
	: prompt_template_(std::move(prompt_template))
	{}

std::string PromptBuilder::build(
	const CodeChunk& chunk) const
{
	std::string prompt = prompt_template_;

	replace(
		prompt,
		"{{SOURCE_FILE}}",
		chunk.sourceFile.string()
	);

	replace(
		prompt,
		"{{CHUNK_INDEX}}",
		std::to_string(chunk.index)
	);

	replace(
		prompt,
		"{{START_LINE}}",
		std::to_string(chunk.start_line)
	);

	replace(
		prompt,
		"{{SOURCE_CODE}}",
		chunk.content
	);
	

	return prompt;
}