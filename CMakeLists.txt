cmake_minimum_required(VERSION 4.1.0)

project(rt-engine LANGUAGES CXX)

include(CTest)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(Ktx REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(renderer)
add_subdirectory(ecs)
add_subdirectory(core)
add_subdirectory(samples)
add_subdirectory(editor)

set_target_properties(samples PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_target_properties(editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT editor)

add_subdirectory(csharp-sandbox)
