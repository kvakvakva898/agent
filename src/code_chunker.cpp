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

			chunks.push_back(std::move(chunk));

			buff.clear();
			line_count = 0;
		}
	}

	if (!buff.empty())
	{
		CodeChunk chunk;

		chunk.sourceFile = file.path;
		chunk.index = index++;
		chunk.content = buff;

		chunks.push_back(std::move(chunk));
	}

	return chunks;
}