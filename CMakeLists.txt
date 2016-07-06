cmake_minimum_required(VERSION 3.5)
project(MatchingEngine)

enable_testing()

set(exename MatchingEngine )
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -Werror" )

#set output folders
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#boost
find_package(Boost 1.59 REQUIRED COMPONENTS unit_test_framework timer) 
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

#include
include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(test)

#get sources
file(GLOB SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cpp" ) 

#executable
add_executable(${exename} ${SOURCES})

add_test(Test${exename} ${CMAKE_BINARY_DIR}/${exename} )

