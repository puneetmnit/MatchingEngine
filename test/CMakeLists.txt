set(test_exe TestOrderBook )

find_package(Boost 1.59 REQUIRED COMPONENTS unit_test_framework timer system)

#boost
#set(Boost_USE_STATIC_LIBS TRUE)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 

#include
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

#get sources
file(GLOB TEST_SOURCES "*.cpp" ) 
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../src/*.cpp")
list(REMOVE_ITEM SOURCES "../src/Main.cpp")

#executable

add_executable(${test_exe} ${TEST_SOURCES} ${SOURCES})
target_link_libraries(${test_exe} ${Boost_LIBRARIES})

add_test(${test_exe} ${EXECUTABLE_OUTPUT_PATH}/${test_exe} )

