#pragma once

#include <cstddef>
#include <string>

struct ReviewIssue
{
	std::string severity;
	std::string category;

	std::size_t line = 0;

	std::string description;
	std::string recommendation;
};