#include "core/project_loader.h"

#include "core/file_scanner.h"
#include "core/file_reader.h"

std::vector<SourceFile> ProjectLoader::load(const std::filesystem::path& path)
{
	FileScanner scanner;
	FileReader reader;

	std::vector<SourceFile> result;


	auto files = scanner.scan(path);

	for (const auto& file : files)
	{
		SourceFile source;

		source.path = file;
		source.content = reader.read(file);

		result.push_back(std::move(source));
	}

	return result;
}