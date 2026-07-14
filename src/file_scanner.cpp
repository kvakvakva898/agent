#include "file_scanner.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> FileScanner::scan(const std::string& root) {
	std::vector<std::string> files;

	try
	{
		for (fs::recursive_directory_iterator it(root,
			fs::directory_options::skip_permission_denied);
			it != fs::recursive_directory_iterator();
			++it)
		{
			if (it->is_directory())
			{
				if (should_skip_dir(it->path().filename().string()))
					it.disable_recursion_pending();
				continue;
			}

			if (!it->is_regular_file())
				continue;

			const std::string path = it->path().string();

			if (is_code_file(path))
				files.push_back(path);
		}
	}
	catch (const fs::filesystem_error& e)
	{
		std::cerr
			<< "Filesystem error: "
			<< e.what()
			<< "\n";
					
	}
	return files;
}

bool FileScanner::is_code_file(const fs::path& path) {
	const auto ext = path.extension();

	return ext == ".cpp" ||
		   ext == ".h" ||
		   ext == ".hpp";
}

bool FileScanner::should_skip_dir(const std::string& name) {
	return name == "out" ||
		   name == "build" ||
		   name == "CMakeFiles" ||
		   name == ".vs" ||
		   name == ".git" ||
		   name == "node_modules";
}
