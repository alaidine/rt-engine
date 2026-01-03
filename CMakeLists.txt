cmake_minimum_required(VERSION 3.31)

project(RoarEngine)

include(CTest)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(raylib REQUIRED)

# Download CPM.cmake
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
        NAME mono_runtime
        URL "https://github.com/alaidine/RoarEngine/releases/download/1.0.0/mono64.zip"
        URL_HASH SHA256=373e23de362a4a97637b55048f4f12dc0cbe44ed63cec94ac2849298f8fdf03b
        DOWNLOAD_ONLY YES
)

CPMAddPackage(
        NAME nativefiledialog
        URL "https://github.com/mlabbe/nativefiledialog/archive/refs/tags/release_116.zip"
        DOWNLOAD_ONLY YES
)

set(MONO_ROOT "${mono_runtime_SOURCE_DIR}")

if (WIN32) # Windows
    set(MONO_INCLUDE_DIR "${MONO_ROOT}/common/include/")
    set(MONO_LIB_DIR "${MONO_ROOT}/windows/")
    set(MONO_ASSEMBLIES "${MONO_ROOT}/common/lib/mono/4.5")
    set(MONO_RUNTIME_LIB "${MONO_LIB_DIR}/mono-2.0-sgen.dll")
    set(MONO_LINK_LIB "${MONO_LIB_DIR}/mono-2.0-sgen.lib")
elseif (UNIX AND NOT APPLE) # Linux
    set(MONO_LIB_DIR "${MONO_ROOT}/linux/")
    set(MONO_RUNTIME_LIB "${MONO_LIB_DIR}/libmonosgen-2.0.so")
    set(MONO_LINK_LIB "${MONO_RUNTIME_LIB}") # On Linux, link directly to .so
endif ()

set(NFD_ROOT "${nativefiledialog_SOURCE_DIR}")
set(NFD_SRC "${NFD_ROOT}/src")
set(NFD_INCLUDE "${NFD_ROOT}/src/include")

set(COMMON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}")

add_subdirectory(renderer)
add_subdirectory(ecs)
add_subdirectory(core)
add_subdirectory(RoarScriptCore)
add_subdirectory(editor)
