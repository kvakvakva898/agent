#pragma once

#include <string>

#include <review_result.h>

class ReviewResultParser
{
public:

	ReviewResult parse(
		const std::string& response
	);
};