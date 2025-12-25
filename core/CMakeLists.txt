add_library(core
    Scripting.cpp
    Scripting.h
    Event.h
    Entrypoints.h
    Inputevents.h
    Window.cpp
    Window.h
    Windowevents.h
    Application.cpp
    Application.h
    Layer.cpp
    Layer.h
    RTEConfig.h)

add_executable(CoreTests
    CoreTests.cpp
)

add_executable(RTEngine
    RTEngine.cpp
    RTEngine.h)

target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/mono/include)
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(core PRIVATE ecs Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

target_include_directories(RTEngine PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(RTEngine PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(RTEngine PRIVATE core Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

set_target_properties(RTEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

target_include_directories(CoreTests PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(CoreTests PRIVATE core Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

add_test(NAME TestCore
    COMMAND ${CMAKE_BINARY_DIR}/core/CoreTests)
