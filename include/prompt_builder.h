#pragma once

#include <string>

#include "code_chunk.h"

class PromptBuilder
{
public:

	explicit PromptBuilder(
		std::string prompt_template
	);

	std::string build(
		const CodeChunk& chunk
	) const;

private:
	std::string prompt_template_;
};