#pragma once

#include <string>

struct ProgramOptions
{
	std::string path;
};

ProgramOptions parse_arguments(int argc, char* argv[]);
void print_usage();