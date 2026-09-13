#pragma once

#include <filesystem>
#include <string>

struct CodeChunk
{
	std::filesystem::path source_file;

	size_t index = 0;

	size_t start_line = 0;

	std::string content;
};