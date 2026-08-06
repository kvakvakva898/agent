#include <iostream>
#include <string>
#include <exception>

#include "program_options.h"
#include "project_loader.h"
#include "code_chunker.h"
#include "prompt_builder.h"

int main(int argc, char* argv[])
{
	ProgramOptions options;

	try
	{
		options = parse_arguments(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr
			<< "Error: "
			<< e.what()
			<< '\n';
		print_usage();
		
		return EXIT_FAILURE;
	}

	ProjectLoader loader;

	std::vector<SourceFile> files;

	try
	{
		files = loader.load(options.path);
	}
	catch (const std::exception& e)
	{
		std::cerr
			<< "Failed to scan project: "
			<< e.what()
			<< '\n';
		return EXIT_FAILURE;
	}

	std::cout
		<< "Found "
		<< files.size()
		<< " source files.\n\n";

	CodeChunker chunker;

	PromptBuilder builder;

	for (const auto& file : files)
	{
		auto chunks = chunker.split(file);

		std::cout
			<< file.path
			<< " -> chunks: "
			<< chunks.size()
			<< '\n';

		for (const auto& chunk : chunks)
		{
			auto prompt = builder.build(chunk);

			std::cout
				<< prompt
				<< "\n\n";
		}

	}

	return EXIT_SUCCESS;
}
