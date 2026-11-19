#pragma once

#include <string>
#include <filesystem>
#include <atomic>
#include <stdexcept>

class RequestCancelled : public std::runtime_error
{
public:
	RequestCancelled()
		: std::runtime_error("Request cancelled")
	{
	}
};

class LLMClient
{
public:
	LLMClient(
		const std::string endpoint,
		const std::string model
	);
	
	std::string chat(
		const std::string& prompt,
		const std::atomic_bool* cancel_requested_ = nullptr
	);

private:
	const std::string endpoint_;
	std::string model_;
};