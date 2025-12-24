add_library(ecs
    ECS.h
    Component.h
    RenderSystem.cpp
    RenderSystem.h)

target_include_directories(ecs PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_link_libraries(ecs PRIVATE renderer Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)
