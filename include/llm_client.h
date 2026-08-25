#pragma once

#include <string>

class LLMClient
{
public:
	LLMClient(
		std::string endpoint,
		std::string model
	);
	
	std::string chat(
		const std::string& prompt
	);

private:
	std::string endpoint_;
	std::string model_;
};