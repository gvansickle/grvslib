cmake_minimum_required(VERSION 3.27)
project(grvslib)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ee)
add_subdirectory(concurrency)

add_library(grvslib STATIC)

target_link_libraries(grvslib ee concurrency)
target_include_directories(grvslib
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		$<INSTALL_INTERFACE:grvslib>
)
