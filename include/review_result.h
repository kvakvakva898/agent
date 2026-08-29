#pragma once

#include <vector>

#include "review_issue.h"

struct ReviewResult
{
	std::vector<ReviewIssue> issues;
};