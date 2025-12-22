add_library(core
    event.h
    entrypoints.h
    inputevents.h
    window.cpp
    window.h
    windowevents.h
    application.cpp
    application.h
    layer.cpp
    layer.h)

add_executable(CoreTests
    CoreTests.cpp
)

target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_link_libraries(core PRIVATE Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

target_include_directories(CoreTests PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_link_libraries(CoreTests PRIVATE core Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

add_test(NAME TestCore
    COMMAND ${CMAKE_BINARY_DIR}/core/CoreTests)
