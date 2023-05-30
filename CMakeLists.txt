cmake_minimum_required(VERSION 3.18)

project(mandelbrot)

add_executable(mandelbrot-cpp)

set_target_properties(mandelbrot-cpp
	PROPERTIES
		CXX_STANDARD_REQUIRED ON
		CXX_STANDARD 17
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/Debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/Release"
		FOLDER "mandelbrot"
)

target_sources(mandelbrot-cpp
	PRIVATE
		"src/Main.cpp"
		"src/Shader.cpp"
		"dependencies/glad/src/glad.c"
)

add_custom_command(TARGET mandelbrot-cpp PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${PROJECT_BINARY_DIR}/$<CONFIG>/shaders"
)

add_custom_command(TARGET mandelbrot-cpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/mandelbrot.vert"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/mandelbrot.frag"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/mandelbrot.comp"
		"${PROJECT_BINARY_DIR}/$<CONFIG>/shaders"
)

# glm
add_subdirectory("dependencies/glm")
set_property(TARGET glm PROPERTY FOLDER "dependencies/glm")

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("dependencies/GLFW")
set(GLFW_LIBRARY "glfw")
set_property(TARGET glfw PROPERTY FOLDER "dependencies/GLFW")
if (TARGET update_mappings)
	set_property(TARGET update_mappings PROPERTY FOLDER "dependencies/GLFW")
endif()

target_include_directories(mandelbrot-cpp
	PRIVATE
		"src/"
		"dependencies/GLFW/include/"
		"dependencies/glad/include/"
		"dependencies/glm/"
)

target_link_libraries(mandelbrot-cpp
	PRIVATE
		"${GLFW_LIBRARY}"
		"glm"
)
