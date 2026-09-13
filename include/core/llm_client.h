#pragma once

#include <string>
#include <filesystem>

class LLMClient
{
public:
	LLMClient(
		const std::string endpoint,
		const std::string model
	);
	
	std::string chat(
		const std::string& prompt
	);

private:
	const std::string endpoint_;
	std::string model_;
};