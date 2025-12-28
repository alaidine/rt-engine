add_executable(editor main.cpp)

target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_link_libraries(editor PRIVATE renderer Vulkan::Vulkan glfw glm::glm KTX::ktx)
target_link_libraries(editor PUBLIC imgui::imgui)

# Add ImGui backend sources (from Dear ImGui backends/).
set(IMGUI_BACKENDS_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui/backends)
if(EXISTS ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp)
	target_sources(editor PRIVATE
		${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp
		${IMGUI_BACKENDS_DIR}/imgui_impl_vulkan.cpp)
	target_include_directories(editor PRIVATE ${IMGUI_BACKENDS_DIR})
endif()
