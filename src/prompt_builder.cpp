#include "prompt_builder.h"

std::string PromptBuilder::build(
	const CodeChunk& chunk
)
{
	std::string prompt;

	prompt +=
		"You are senior C++ developer.\n\n";

	prompt +=
		"Analyze the following code.\n\n";

	prompt +=
		"Find:\n"
		"- bugs\n"
		"- memory issues\n"
		"- perfomance problems\n"
		"- bad practices\n\n";

	prompt +=
		"Return your answer as structured text.\n\n";

	prompt +=
		"File: "
		+ chunk.sourceFile.string()
		+ "\n";

	prompt +=
		"Code:\n";

	prompt += chunk.content;

	return prompt;
}