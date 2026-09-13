#pragma once

#include <string>
#include <filesystem>

struct ProgramOptions
{
	std::filesystem::path path;

	std::string endpoint =
		"http://10.66.0.115:1234/v1/chat/completions";

	std::string model =
		"nvidia/nemotron-3-nano-4b";
};

ProgramOptions parse_arguments(int argc, char* argv[]);
void print_usage();