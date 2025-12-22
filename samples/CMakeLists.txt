add_executable(samples main.cpp)

target_include_directories(samples PRIVATE ${CMAKE_SOURCE_DIR}/renderer ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(samples PRIVATE renderer core Vulkan::Vulkan glfw glm::glm KTX::ktx)
