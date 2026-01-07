add_library(renderer
    Renderer.h
    Renderer.cpp)

target_include_directories(renderer PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(renderer PRIVATE raylib glm::glm)
