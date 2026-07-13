#include "program_options.h"
#include <exception>
#include <iostream>

ProgramOptions parse_arguments(int argc, char* argv[])
{
	ProgramOptions options;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (arg == "--path")
		{
			if (i + 1 >= argc)
				throw std::runtime_error("Missing value for --path");

			options.path = argv[++i];
		}
	}

	if (options.path.empty())
		throw std::runtime_error("Project path is nor specified.");

	return options;
}

void print_usage()
{
	std::cout << "Usage: "
			  << "agent --path <project>\n>";
}