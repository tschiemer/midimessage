cmake_minimum_required(VERSION 3.3)
project(midimessage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES main.cpp include/midimessage.h src/midimessage.cpp)

add_library(midimessage ${SOURCE_FILES})


add_executable(tester test/tester.cpp)
target_link_libraries(tester midimessage)
