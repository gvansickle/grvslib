cmake_minimum_required(VERSION 3.25)

# Prevent an in-root-of-src-tree build and any accidental build-induced changes to source files.
# Per @link https://stackoverflow.com/questions/1208681/with-cmake-how-would-you-disable-in-source-builds, these have to
# be prior to project().
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(grvslib CXX)

if((${CMAKE_CXX_COMPILER_ID} STREQUAL Clang) AND (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS_EQUAL 14))
	message("Clang version <= 14 can't compile gtest at std > 17")
	set(CMAKE_CXX_STANDARD 17)
else ()
	set(CMAKE_CXX_STANDARD 23)
endif ()
set(CMAKE_CXX_STANDARD_REQUIRED Yes)

add_subdirectory(ee)
add_subdirectory(concurrency)

add_library(grvslib STATIC)
target_link_libraries(grvslib
	PRIVATE
		ee
		concurrency)
target_include_directories(grvslib
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		$<INSTALL_INTERFACE:grvslib>
)

# Pull in GoogleTest
include(FetchContent)
FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        main
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

###
### Install Section
###
### @todo

###
### Testing section
###
enable_testing()
include(GoogleTest)
add_subdirectory(tests)


### Packaging section
### @todo
