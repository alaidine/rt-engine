add_library(renderer
    extras/FA6FreeSolidFontData.h
    extras/IconsFontAwesome6.h
    imgui_impl_raylib.h
    rlImGui.cpp
    rlImGui.h
    rlImGuiColors.h
    Renderer.h
    Renderer.cpp)

target_include_directories(renderer PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(renderer PRIVATE raylib glm::glm imgui::imgui)
