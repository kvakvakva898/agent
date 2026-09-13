#include "core/code_review_agent.h"

#include <iostream>
#include <stdexcept>

CodeReviewAgent::CodeReviewAgent(
	const std::string& endpoint,
	const std::string& model,
	const std::filesystem::path& prompt_template_path)
	: prompt_builder_(
		PromptTemplateLoader{}.load(prompt_template_path)),
	  llm_client_(
		  endpoint,
		  model)
{
}


std::vector<ReviewIssue> CodeReviewAgent::analyze(
	const std::filesystem::path& project_path)
{
	std::vector<ReviewIssue> all_issues;

	//1.Загружаем исходные файлы проекта

	const auto files =
		project_loader_.load(project_path);

	std::cout
		<< "Found "
		<< files.size()
		<< " source files.";

	for (const auto& file : files)
	{
		auto chunks = chunker_.split(file);

		std::cout
			<< file.path
			<< " -> chunks: "
			<< chunks.size()
			<< '\n';

		//3. Обрабатываем каждый полученный chunk
		for (const auto& chunk : chunks)
		{

			std::cout
				<< "\n==================================\n"
				<< "Analyzing: "
				<< chunk.source_file
				<< " chunk["
				<< chunk.index
				<< "]"
				<< "\n==================================\n\n";

			//4. Строим prompt
			auto prompt = 
				prompt_builder_.build(chunk);

			try
			{

				//5. Отправляем prompt LLM
				const auto response = llm_client_.chat(prompt);

				try
				{
					//6. Парсим ответ модели
					auto review = result_parser_.parse(response);

					std::cout
						<< "Found issues: "
						<< review.issues.size()
						<< '\n';

					for (auto& issue: review.issues)
					{
						issue.source_file =
							chunk.source_file;

						all_issues.push_back(
							std::move(issue)
						);
					}
				}
				catch (const std::exception& e)
				{
					std::cerr
						<< "Failed to parse LLM response: "
						<< e.what()
						<< '\n';
				}
			}
			catch (const std::exception& e)
			{
				std::cerr
					<< "LLM request failed: "
					<< e.what()
					<< '\n';
			}
		}
	}

	return all_issues;
}