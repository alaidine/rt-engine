cmake_minimum_required(VERSION 3.24...4.1)
project(r-type)

include(FetchContent)

set(CLIENT r-type_client)
set(SERVER r-type_server)

set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

add_executable(${CLIENT})
add_executable(${SERVER})
add_subdirectory(src)

set_target_properties(${CLIENT} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})


set_target_properties(${SERVER} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${CLIENT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${CLIENT}>)
set_property(TARGET ${SERVER} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${SERVER}>)

add_custom_command(
    TARGET ${CLIENT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${CLIENT}>/resources
)
#DEPENDS ${PROJECT_NAME}

#set(raylib_VERBOSE 1)
target_link_libraries(${CLIENT} raylib)
target_link_libraries(${SERVER} raylib)

# Platform-specific libraries
if(WIN32)
    # Windows-specific libraries (Winsock for networking)
    target_link_libraries(${CLIENT} ws2_32)
    target_link_libraries(${SERVER} ws2_32)
elseif(UNIX AND NOT APPLE)
    # Linux-specific libraries
    target_link_libraries(${CLIENT} pthread)
    target_link_libraries(${SERVER} pthread)
endif()
