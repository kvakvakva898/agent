#pragma once

#include "source_file.h"
#include "code_chunk.h"

class CodeChunker
{
public:

	std::vector<CodeChunk> split(const SourceFile& file);

private:

	static constexpr size_t MAX_LINES = 200;
};