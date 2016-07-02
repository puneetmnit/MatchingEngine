set(test_exe TestOrderBook )

#boost
#set(Boost_USE_STATIC_LIBS TRUE)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 

#include
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

#get sources
file(GLOB SOURCES "*.cpp" ) 

#executable
add_executable(${test_exe} ${SOURCES})
target_link_libraries(${test_exe} ${Boost_LIBRARIES})

message(STATUS "binary path: ${EXECUTABLE_OUTPUT_PATH}" ) 
add_test(${test_exe} ${EXECUTABLE_OUTPUT_PATH}/${test_exe} )

