cmake_minimum_required(VERSION 3.3)
project(midimessage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES include/midimessage.h src/midimessage.cpp)

add_library(midimessage ${SOURCE_FILES})


add_executable(example-sample example/sample.cpp)
target_link_libraries(example-sample midimessage)

add_custom_target(manufacturerids
        COMMAND wget https://www.midi.org/specifications-old/item/manufacturer-id-numbers
        COMMAND php ${CMAKE_CURRENT_SOURCE_DIR}/tools/createManufacturerIdsHeader.php manufacturer-id-numbers ${CMAKE_CURRENT_SOURCE_DIR}/include/manufacturerids.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/manufacturerids.h
        COMMAND rm manufacturer-id-numbers)

message("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")
message("NOTE to create an up-to-date 'include/manufacturerids.h' you can 'make manufacturerids' (requires wget and php)")
message("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
