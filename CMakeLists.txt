# CMakeList.txt: проект CMake для agent; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (agent LANGUAGES CXX)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (agent  
	src/main.cpp
	src/file_reader.cpp
	src/file_scanner.cpp
	src/program_options.cpp
	src/project_loader.cpp
	src/code_chunker.cpp
	src/prompt_builder.cpp
	src/llm_client.cpp
	src/review_result_parser.cpp
	src/prompt_template_loader.cpp
	src/code_review_agent.cpp)

target_include_directories(agent PRIVATE include)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(agent
	PRIVATE
		CURL::libcurl
		nlohmann_json::nlohmann_json)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
