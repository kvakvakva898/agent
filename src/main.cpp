#include <iostream>
#include <string>
#include <exception>

#include "program_options.h"
#include "project_loader.h"

int main(int argc, char* argv[])
{
	ProgramOptions options;

	try
	{
		options = parse_arguments(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: "
				  << e.what()
				  << '\n';
		print_usage();
		
		return 1;
	}

	ProjectLoader loader;

	std::vector<SourceFile> files;

	try
	{
		files = loader.load(options.path);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to scan project: "
				  << e.what()
				  << '\n';
		return 1;
	}

	std::cout << "Found "
			  << files.size()
			  << " source files.\n\n";

	for (const auto& file : files)
	{

		std::cout << file.content;

		if (!file.content.empty() && file.content.back() != '\n');
		{
			std::cout << '\n';
		}
	}

	return 0;
}
