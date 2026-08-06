#pragma once

#include <string>

#include "code_chunk.h"

class PromptBuilder
{
public:

	std::string build(
		const CodeChunk& chunk
	);
};