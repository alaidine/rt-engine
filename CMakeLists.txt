cmake_minimum_required(VERSION 3.28)

project(RoarEngine)

set(CMAKE_CXX_STANDARD 20)

# Download CPM.cmake
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMFindPackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 1.0.3
)

CPMFindPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  GIT_TAG v1.17.0
)

CPMFindPackage(
  NAME raylib
  GITHUB_REPOSITORY raysan5/raylib
  GIT_TAG 5.5
  OPTIONS "BUILD_EXAMPLES OFF"
)

CPMFindPackage(
  NAME asio
  GITHUB_REPOSITORY chriskohlhoff/asio
  GIT_TAG asio-1-36-0
)

set(COMMON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}")

add_subdirectory(renderer)
add_subdirectory(networking)
add_subdirectory(ecs)
add_subdirectory(core)
