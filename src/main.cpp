#include <iostream>
#include <string>
#include <exception>

#include "program_options.h"
#include "project_loader.h"
#include "code_chunker.h"
#include "prompt_builder.h"
#include "llm_client.h"
#include "review_result_parser.h"

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
	ReviewResultParser parser;

	LLMClient client(
		options.endpoint,
		options.model
	);

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
				<< "\n==================================\n"
				<< "Analyzing: "
				<< chunk.sourceFile
				<< " chunk["
				<< chunk.index
				<< "]"
				<< "\n==================================\n\n";

			try
			{
				const auto response = client.chat(prompt);

				try
				{
					const auto review = parser.parse(response);

					std::cout
						<< "Found issues: "
						<< review.issues.size()
						<< '\n';

					for (const auto& issue : review.issues)
					{
						std::cout
							<< '\n'
							<< "["
							<< issue.severity
							<< "] "
							<< issue.category
							<< '\n';

						std::cout
							<< "Line: "
							<< issue.line
							<< '\n';

						std::cout
							<< "Description: "
							<< issue.description
							<< '\n';

						std::cout
							<< "Recomendation: "
							<< issue.recommendation
							<< '\n';
					}
				}
				catch (const std::exception& e)
				{
					std::cerr
						<< "Failed to parse LLM response: "
						<< e.what()
						<< 'n';

					std::cerr
						<< "Raw response:\n"
						<< response
						<< 'n';

					return EXIT_FAILURE;
				}
			}
			catch (const std::exception& e)
			{
				std::cerr
					<< "LLM request failed: "
					<< e.what()
					<< '\n';

				return EXIT_FAILURE;
			}
		}

	}

	return EXIT_SUCCESS;
}
