#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class FileScanner {
public:
	std::vector<std::string> scan(const std::string& root);

private:
	bool is_code_file(const fs::path& path);
	bool should_skip_dir(const std::string& name);
};