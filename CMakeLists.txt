cmake_minimum_required(VERSION 4.1.0)

project(rt-engine)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
  NAME lua
  VERSION 5.3.5
  URL https://github.com/lua/lua/archive/refs/tags/v5.4.8.tar.gz
  DOWNLOAD_ONLY YES
)

if(lua_ADDED)
  # lua has no CMakeLists, so we create our own target
  file(GLOB lua_sources ${lua_SOURCE_DIR}/*.c)
  list(REMOVE_ITEM lua_sources "${lua_SOURCE_DIR}/lua.c" "${lua_SOURCE_DIR}/luac.c")
  add_library(lua STATIC ${lua_sources})
  target_include_directories(lua SYSTEM PUBLIC $<BUILD_INTERFACE:${lua_SOURCE_DIR}>)
endif()

CPMAddPackage(
  NAME sol2
  VERSION 3.3.0
  URL https://github.com/ThePhD/sol2/archive/refs/tags/v3.3.0.tar.gz
)

CPMAddPackage(
    NAME glm
    VERSION 1.0.2
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.2.tar.gz
)

CPMAddPackage(
    NAME ktx
    URL https://github.com/KhronosGroup/KTX-Software/archive/refs/tags/v4.4.2.tar.gz
    OPTIONS "KTX_GIT_VERSION_FULL v4.4.2"
)

CPMAddPackage(
    NAME glfw
    VERSION 3.4
    URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
)

add_subdirectory(renderer)
add_subdirectory(samples)

set_target_properties(samples PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT samples)
