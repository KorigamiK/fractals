cmake_minimum_required(VERSION 3.14)

project(
	fractals
	VERSION 0.1.0
	DESCRIPTION "Fractals"
	HOMEPAGE_URL "https://korigamik.ml/fractals"
	LANGUAGES CXX
)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(utils)

add_subdirectory(explorer)

enable_testing()

add_subdirectory(test)

add_custom_target(
	check
	COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
	DEPENDS test
)

set(GENERATE_DOCS ON CACHE BOOL "Generate documentation")

if(GENERATE_DOCS)

	find_package(Doxygen REQUIRED)
	set(DOXYGEN_GENERATE_HTML YES)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile @ONLY)
	add_custom_target(
		doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM
	)

endif()