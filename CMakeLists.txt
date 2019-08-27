cmake_minimum_required(VERSION 3.3)
project(midimessage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

set(SOURCE_FILES
        include/midimessage.h
        include/midimessage-packers.h
        include/manufacturerids.h
        include/dlsproprietarychunkids.h
        include/commonccs.h
        include/notes.h
        src/midimessage.cpp)

add_library(midimessage ${SOURCE_FILES})

add_executable(midimessage-cli src/cli.cpp)
target_link_libraries(midimessage-cli midimessage)


message("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")
message("NOTE to create an up-to-date 'include/manufacturerids.h' you can 'make manufacturerids' (requires wget and php)")
message("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")

add_custom_target(manufacturerids
        COMMAND wget https://www.midi.org/specifications-old/item/manufacturer-id-numbers
        COMMAND php ${CMAKE_CURRENT_SOURCE_DIR}/tools/createManufacturerIdsHeader.php manufacturer-id-numbers ${CMAKE_CURRENT_SOURCE_DIR}/include/manufacturerids.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/manufacturerids.h
        COMMAND rm manufacturer-id-numbers)

add_custom_target(docs
        COMMAND doxygen doxygen.config)

# EXAMPLES

add_executable(simple EXCLUDE_FROM_ALL examples/simple.cpp)
target_link_libraries(simple midimessage)

set_target_properties(simple
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/bin"
        )

# Group examples into target <examples>
if(NOT TARGET examples)
    add_custom_target(examples)
    add_dependencies(examples simple)
endif()
