cmake_minimum_required(VERSION 3.3)
project(midimessage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
        include
        PRIVATE deps/c-utils/lib)

set(HEADER_FILES
        include/midimessage.h
        include/midimessage/packers.h
        include/midimessage/manufacturerids.h
        include/midimessage/dlsproprietarychunkids.h
        include/midimessage/commonccs.h
        include/midimessage/notes.h
        include/midimessage/stringifier.h
        include/midimessage/parser.h)

set(SOURCE_FILES
        src/midimessage.cpp
        src/stringifier.cpp
        src/parser.cpp
        deps/c-utils/lib/util-hex.c)

add_library(midimsg STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_executable(midimessage src/cli.cpp )
target_link_libraries(midimessage midimsg)

#target_include_directories(midimessage-cli PRIVATE deps/c-utils/lib)

message("Install path: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS midimsg midimessage
        LIBRARY
            DESTINATION lib
            COMPONENT Libraries
            NAMELINK_COMPONENT Development
        ARCHIVE
            DESTINATION lib
#        PUBLIC_HEADER
#            DESTINATION include
#            COMPONENT Development
        RUNTIME
            DESTINATION bin
            COMPONENT Runtime
)
install(FILES
            include/midimessage.h
#        PUBLIC_HEADER
        DESTINATION include
        COMPONENT Development
)
install(FILES
            include/midimessage/packers.h
            include/midimessage/manufacturerids.h
            include/midimessage/dlsproprietarychunkids.h
            include/midimessage/commonccs.h
            include/midimessage/notes.h
            include/midimessage/stringifier.h
            include/midimessage/parser.h
#        PUBLIC_HEADER
        DESTINATION include/midimessage
        COMPONENT Development
)


message("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")
message("NOTE to create an up-to-date 'include/manufacturerids.h' you can 'make manufacturerids' (requires wget and php)")
message("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")

add_custom_target(manufacturerids
        COMMAND wget https://www.midi.org/specifications-old/item/manufacturer-id-numbers
        COMMAND php ${CMAKE_CURRENT_SOURCE_DIR}/tools/createManufacturerIdsHeader.php manufacturer-id-numbers ${CMAKE_CURRENT_SOURCE_DIR}/include/midimessage/manufacturerids.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/midimessage/manufacturerids.h
        COMMAND rm manufacturer-id-numbers)

add_custom_target(docs
        COMMAND doxygen)

# EXAMPLES

add_executable(simple EXCLUDE_FROM_ALL examples/simple.cpp)
target_link_libraries(simple midimsg)

set_target_properties(simple
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/bin"
        )

# Group examples into target <examples>
if(NOT TARGET examples)
    add_custom_target(examples)
    add_dependencies(examples simple)
endif()
