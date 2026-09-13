#pragma once

#include <cstddef>
#include <string>
#include <filesystem>

struct ReviewIssue
{
	std::filesystem::path source_file;
	std::string severity;
	std::string category;

	std::size_t line = 0;

	double confidence = 0.0;

	std::string description;
	std::string recommendation;
};