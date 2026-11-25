	# CMakeList.txt: проект CMake для agent; включите исходный код и определения,
	# укажите здесь логику для конкретного проекта.
	#
	cmake_minimum_required (VERSION 3.20)

	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)

	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)

	# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
	if (POLICY CMP0141)
	  cmake_policy(SET CMP0141 NEW)
	  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
	endif()

	set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

	project (agent LANGUAGES CXX)

	find_package(CURL REQUIRED)
	find_package(nlohmann_json CONFIG REQUIRED)
	find_package(Qt6 REQUIRED COMPONENTS Widgets)

	add_library(
		agent_core
		STATIC

		src/core/file_reader.cpp
		src/core/file_scanner.cpp
		src/core/project_loader.cpp

		src/core/code_chunker.cpp

		src/core/prompt_builder.cpp
		src/core/prompt_template_loader.cpp

		src/core/llm_client.cpp
		src/core/review_result_parser.cpp
		src/core/code_review_service.cpp
		src/core/config_loader.cpp
	
		src/app/application.cpp
	)

	target_include_directories(
		agent_core
		PUBLIC
		include
	)

	target_link_libraries(
		agent_core
		PUBLIC
			CURL::libcurl
			nlohmann_json::nlohmann_json
	)

	add_executable (
		agent-cli

		src/cli/main.cpp
		src/cli/program_options.h
		src/cli/program_options.cpp
		)

	target_include_directories(
		agent-cli
		PRIVATE
		/src/cli
	)

	target_link_libraries(
		agent-cli
		PRIVATE
			agent_core
	)

	add_executable(
		agent-gui

		src/gui/main.cpp
		include/gui/main_window.h
		src/gui/main_window.cpp
		include/gui/review_worker.h
		src/gui/review_worker.cpp
	)

	target_link_libraries(
		agent-gui
		PRIVATE
			agent_core
			Qt6::Widgets
	)

	
	install(TARGETS agent-gui
		BUNDLE DESTINATION .
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

	install(
		DIRECTORY "${CMAKE_SOURCE_DIR}/prompts/"
		DESTINATION "${CMAKE_INSTALL_BINDIR}/prompts"
	)

	qt_generate_deploy_app_script(
		TARGET agent-gui
		OUTPUT_SCRIPT deploy_script
		NO_UNSUPPORTED_PLATFORM_ERROR
	)

	install(SCRIPT ${deploy_script})