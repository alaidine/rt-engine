add_library(ecs
    ECS.h
    Component.h
    ScriptSystem.cpp
    ScriptSystem.h
    RenderSystem.cpp
    RenderSystem.h)

target_include_directories(ecs PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(ecs PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_include_directories(ecs PRIVATE ${MONO_INCLUDE_DIR})
target_link_libraries(ecs PRIVATE renderer core raylib glm::glm imgui::imgui ${MONO_LIB_DIR}/mono-2.0-sgen.lib spdlog::spdlog)
