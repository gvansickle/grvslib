cmake_minimum_required(VERSION 3.27)
project(grvslib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Pull in GoogleTest
## Note: This "FetchContent" method is what the GT docs recommend (https://google.github.io/googletest/quickstart-cmake.html),
## but they're now running live at head, so it seems we should really be using a subrepo, similar to:
## https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
include(FetchContent)
FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

### Install Section
### @todo

###
### Testing section
###
enable_testing()
include(GoogleTest)
add_subdirectory(tests)


### Packaging section
### @todo
