cmake_minimum_required(VERSION 3.3)
project(midimessage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

set(SOURCE_FILES
        include/midimessage.h
        include/manufacturerids.h
        include/commonccs.h
        include/notes.h
        include/mmstdlib.h
        src/midimessage.cpp)

add_library(midimessage ${SOURCE_FILES})

add_executable(midimessage-cli src/cli.cpp src/mmstdlib.cpp)
target_link_libraries(midimessage-cli midimessage)

add_executable(simple examples/simple.cpp src/mmstdlib.cpp)
target_link_libraries(simple midimessage)

set_target_properties(simple
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
        )

add_custom_target(manufacturerids
        COMMAND wget https://www.midi.org/specifications-old/item/manufacturer-id-numbers
        COMMAND php ${CMAKE_CURRENT_SOURCE_DIR}/tools/createManufacturerIdsHeader.php manufacturer-id-numbers ${CMAKE_CURRENT_SOURCE_DIR}/include/manufacturerids.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/manufacturerids.h
        COMMAND rm manufacturer-id-numbers)

message("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")
message("NOTE to create an up-to-date 'include/manufacturerids.h' you can 'make manufacturerids' (requires wget and php)")
message("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")
