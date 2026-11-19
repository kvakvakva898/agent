#include "core/code_review_service.h"

#include <iostream>
#include <stdexcept>

CodeReviewService::CodeReviewService(
	ProjectLoader& project_loader,
	CodeChunker& chunker,
	PromptBuilder& prompt_builder,
	LLMClient& llm_client,
	ReviewResultParser& result_parser)
	: project_loader_(project_loader),
	  chunker_(chunker),
	  prompt_builder_(prompt_builder),
	  llm_client_(llm_client),
	  result_parser_(result_parser)
{
}


std::vector<ReviewIssue> CodeReviewService::analyze(
	const std::filesystem::path& project_path,
	std::function<void(const std::string&)> progress,
	const std::atomic_bool* cancel_requested)
{
	const auto is_cancelled =
		[&]()
		{
			return cancel_requested &&
				   cancel_requested->load(
					   std::memory_order_relaxed);
		};

	if (progress)
	{
		progress("Scanning project...");
	}
	std::vector<ReviewIssue> all_issues;

	//1.Загружаем исходные файлы проекта

	const auto files =
		project_loader_.load(project_path);

	if (is_cancelled())
		return {};

	if (progress)
	{
		progress(
			"Found " +
			std::to_string(files.size()) +
			" source files.");
	}

	for (std::size_t file_index = 0;
		file_index < files.size();
		++file_index)
	{
		if (is_cancelled())
			return {};

		const auto& file = files[file_index];

		auto chunks = chunker_.split(file);

		if (progress)
		{
			progress(
				"Analyzing " +
				file.path.string() +
				" (" +
				std::to_string(file_index + 1) +
				"/" +
				std::to_string(files.size()) +
				")");
		}

		//3. Обрабатываем каждый полученный chunk
		for (const auto& chunk : chunks)
		{
			if (is_cancelled())
				return {};

			//4. Строим prompt
			auto prompt =
				prompt_builder_.build(chunk);

			//5. Отправляем prompt LLM
			const auto response = llm_client_.chat(
				prompt,
				cancel_requested);


			//6. Парсим ответ модели
			auto review = result_parser_.parse(response);

			for (auto& issue : review.issues)
			{
				issue.source_file =
					chunk.source_file;

				all_issues.push_back(
					std::move(issue)
				);
			}

		}
	}

	if (is_cancelled())
		return {};

	return all_issues;
}