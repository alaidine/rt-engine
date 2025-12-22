add_library(renderer
    benchmark.hpp
    camera.hpp
    CommandLineParser.hpp
    Entrypoint.h
    keycodes.hpp
    VulkanBuffer.cpp
    VulkanBuffer.h
    VulkanDebug.cpp
    VulkanDebug.h
    VulkanDevice.cpp
    VulkanDevice.h
    VulkanInitializers.hpp
    VulkanSwapChain.cpp
    VulkanSwapChain.h
    VulkanTools.h
    VulkanTools.cpp
    VulkanUIOverlay.cpp
    VulkanUIOverlay.h
    Renderer.h
    Renderer.cpp)

target_link_libraries(renderer PRIVATE Vulkan::Vulkan glfw glm::glm KTX::ktx)
target_link_libraries(renderer PUBLIC imgui::imgui)
