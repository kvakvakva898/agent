#include "review_result_parser.h"

#include <nlohmann/json.hpp>

#include <stdexcept>

using json = nlohmann::json;

ReviewResult ReviewResultParser::parse(
	const std::string& response)
{
	json data;

	try
	{
		data = json::parse(response);
	}
	catch (const json::parse_error& e)
	{
		throw std::runtime_error(
			std::string("LLM returned invalid JSON: ")
			+ e.what()
		);
	}

	if (!data.contains("issues"))
	{
		throw std::runtime_error(
			"'issues' must be a JSON array"
		);
	}

	if (!data["issues"].is_array())
	{
		throw std::runtime_error(
			"'issues' must be a JSON array"
		);
	}

	ReviewResult result;

	for (const auto& item : data["issues"])
	{
		if (!item.is_object())
		{
			throw std::runtime_error(
				"Each issue must be a JSON object"
			);
		}

		ReviewIssue issue;

		issue.severity =
			item.value("severity", "unknown");

		issue.category =
			item.value("category", "unknown");

		issue.line =
			item.value("line", 0);

		issue.confidence =
			item.value("confidence", 0.0);

		if (issue.confidence < 0.0 || 
			issue.confidence > 1.0)
		{
			throw std::runtime_error(
				"Issue confidence must be between 0.0 and 1.0"
			);
		}

		issue.description =
			item.value("description", "");

		issue.recommendation =
			item.value("recomendation", "");

		result.issues.push_back(
			std::move(issue)
		);
	}

	return result;
}