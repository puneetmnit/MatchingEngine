cmake_minimum_required(VERSION 3.5)
project(MatchingEngine)

set(CMAKE_CXX_FLAGS "-std=c++14" )

#set output folders
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#include
include_directories("${PROJECT_SOURCE_DIR}")

#executable
add_executable( ${exename} ${source} )

