#pragma once

#include <filesystem>
#include <vector>
#include "source_file.h"

class ProjectLoader
{
public:
	std::vector<SourceFile> load(const std::filesystem::path& path);
};