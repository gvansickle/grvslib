
add_subdirectory(ee)
add_subdirectory(concurrency)

add_library(grvslib STATIC)
target_sources(grvslib
	PUBLIC
		linkme.cpp
)
target_link_libraries(grvslib
		PRIVATE
		ee
		concurrency)
target_include_directories(grvslib
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		$<INSTALL_INTERFACE:grvslib>
)
