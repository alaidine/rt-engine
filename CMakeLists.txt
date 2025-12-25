cmake_minimum_required(VERSION 3.31)

project(rt-engine LANGUAGES CXX)

include(CTest)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(Ktx REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)

# Put all runtime/library/archive outputs into the same directory
set(RT_ENGINE_OUT_DIR ${CMAKE_BINARY_DIR}/RTEngine)

# Single-config generators
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY     ${RT_ENGINE_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY     ${RT_ENGINE_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY     ${RT_ENGINE_OUT_DIR})
set(CMAKE_PDB_OUTPUT_DIRECTORY         ${RT_ENGINE_OUT_DIR})

# Multi-config generators (e.g., Visual Studio)
foreach(cfg IN LISTS CMAKE_CONFIGURATION_TYPES)
  string(TOUPPER "${cfg}" _cfg)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg} ${RT_ENGINE_OUT_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_cfg} ${RT_ENGINE_OUT_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_cfg} ${RT_ENGINE_OUT_DIR})
  set(CMAKE_PDB_OUTPUT_DIRECTORY_${_cfg} ${RT_ENGINE_OUT_DIR})
endforeach()

add_subdirectory(renderer)
add_subdirectory(ecs)
add_subdirectory(core)
add_subdirectory(samples)
add_subdirectory(editor)
add_subdirectory(csharp-sandbox)
add_subdirectory(rtmodule)

set_target_properties(samples PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT editor)
