#include "core/file_reader.h"

#include <fstream>
#include <sstream>

std::string FileReader::read(const std::string& path) {

	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("Error with file opening");

	std::stringstream buff;

	buff << file.rdbuf();

	return buff.str();
}