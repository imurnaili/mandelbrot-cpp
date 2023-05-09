cmake_minimum_required(VERSION 3.18)

project(mandelbrot)

add_executable(mandelbrot-cpp)

target_sources(mandelbrot-cpp
	PRIVATE
		"src/Main.cpp"
)

target_include_directories(mandelbrot-cpp
	PRIVATE
		"src/"
)
