#include "prompt_builder.h"

std::string PromptBuilder::build(
	const CodeChunk& chunk
)
{
	std::string prompt;

	prompt +=
		"You are a senior C++ code reviewer.\n\n";

	prompt +=
		"Analyze the provided C++ source code and identify "
		"real and actionable problems.\n\n";

	prompt +=
		"Look for:\n"
		"- bugs\n"
		"- memory safety issues\n"
		"- undefined behavior\n"
		"- performance problems\n"
		"- concurrency issues\n"
		"- incorrect resource management\n"
		"- serious maintainability problems\n";

	prompt +=
		"Do not report harmless stylistic preferences.\n";

	prompt +=
		"Do not invent problems that are not supported by the code.\n";

	prompt +=
		"Return only valid JSON.\n";

	prompt +=
		"The JSON must have exactly this structure.\n";

	prompt +=
		"{\n"
		"	\"issues\": [\n"
		"		{\n"
		"			\"severity\": \"high|medium|low\",\n"
		"			\"category\": \"bug|memory|perfomance|concurrency|resource|maintainability\",\n"
		"			\"line\": 0,\n"
		"			\"description\": \"...\",\n"
		"			\"recommendation\": \"...\"\n"
		"		}\n"
		"	]\n"
		"}\n\n";

	prompt +=
		"If there are no issues, return:\n\n";

	prompt +=
		"{ \"issues\": [] }\n\n";

	prompt +=
		"File: "
		+ chunk.sourceFile.string()
		+ "\n";

	prompt +=
		"Code:\n";

	prompt += chunk.content;

	return prompt;
}