project(
	"Explorer"
	VERSION 0.1.0
)

file(GLOB_RECURSE SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "include/*.h")

message(STATUS "Sources: ${SOURCES}")
message(STATUS "Headers: ${HEADERS}")

set(DEFINES "")

add_executable(Explorer ${SOURCES} ${HEADERS})

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		OpenGL::OpenGL
		SDL2::SDL2
		GLEW::GLEW
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		# -Werror
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		${DEFINES}
)

# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
