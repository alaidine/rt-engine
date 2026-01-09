add_library(renderer
    framework.h
    Renderer.h
    Renderer.cpp)

target_include_directories(renderer PUBLIC ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(renderer PUBLIC raylib glm::glm)
