cmake_minimum_required(VERSION 3.28)

project(r-type)

# Download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.2/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=c8cdc32c03816538ce22781ed72964dc864b2a34a310d3b7104812a5ca2d835d
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
  NAME RoarEngine
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../"
)

add_executable(r-type_server server.cpp)
add_executable(r-type_client client.cpp)

target_link_libraries(r-type_server PUBLIC RoarEngine)
target_link_libraries(r-type_client PUBLIC RoarEngine)
