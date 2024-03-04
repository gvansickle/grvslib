cmake_minimum_required(VERSION 3.27)
project(grvslib)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ee)

add_library(grvslib STATIC)

target_link_libraries(grvslib ee)
