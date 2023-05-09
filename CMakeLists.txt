cmake_minimum_required(VERSION 3.18)

project(mandelbrot)

add_executable(mandelbrot-cpp)

target_sources(mandelbrot-cpp
	PRIVATE
		"src/Main.cpp"
		"src/Shader.cpp"
		"src/Build.cpp"
		"dependencies/glad/src/glad.c"
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
		"dependencies/stb/"
)

target_link_libraries(mandelbrot-cpp
	PRIVATE
		"${GLFW_LIBRARY}"
		"glm"
)
