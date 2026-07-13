#include <iostream>
#include <string>
#include <exception>

#include "file_scanner.h"
#include "file_reader.h"
#include "program_options.h"

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

	FileScanner scanner;
	FileReader reader;

	std::vector<std::string> files;

	try
	{
		files = scanner.scan(options.path);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to scan projectL "
				  << e.what()
				  << '\n';
		return 1;
	}

	std::cout << "Found "
			  << files.size()
			  << " source files.\n\n";

	for (const auto& file : files)
	{
		std::cout << "=========================================================\n"
				  << file
				  << "\n=========================================================\n\n";

		const auto content = reader.read(file);

		std::cout << content;

		if (!content.empty() && content.back() != '\n');
		{
			std::cout << '\n';
		}
	}

	return 0;
}
