#include "code_chunker.h"

#include <sstream>

std::vector<CodeChunk> CodeChunker::split(const SourceFile& file)
{
	std::vector<CodeChunk> chunks;

	std::stringstream stream(file.content);

	std::string line;

	std::string buff;

	size_t line_count = 0;
	size_t index = 0;
	size_t start_line = 1;
	size_t current_line = 1;

	while (std::getline(stream, line))
	{
		buff += line;
		buff += '\n';

		line_count++;

		if (line_count >= MAX_LINES)
		{
			CodeChunk chunk;

			chunk.sourceFile = file.path;
			chunk.index = index++;
			chunk.content = buff;
			chunk.start_line = start_line;

			chunks.push_back(std::move(chunk));

			buff.clear();
			line_count = 0;
			start_line = current_line + 1;
		}

		++current_line;
	}

	if (!buff.empty())
	{
		CodeChunk chunk;

		chunk.sourceFile = file.path;
		chunk.index = index++;
		chunk.content = buff;
		chunk.start_line = start_line;

		chunks.push_back(std::move(chunk));
	}

	return chunks;
}