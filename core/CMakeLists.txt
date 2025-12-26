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
target_link_libraries(core PRIVATE ecs Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui spdlog::spdlog)

target_include_directories(CoreTests PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(CoreTests PRIVATE core Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

add_test(NAME TestCore
    COMMAND ${CMAKE_BINARY_DIR}/core/CoreTests)

set(MONO_LIB_DIR "${CMAKE_SOURCE_DIR}/mono/lib")
set(MONO_SGEN_LIB_PATH "${MONO_LIB_DIR}/mono-2.0-sgen.lib")

target_link_libraries(core PRIVATE "${MONO_SGEN_LIB_PATH}")

target_include_directories(RTEngine PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(RTEngine PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(RTEngine PRIVATE core Vulkan::Vulkan boost::boost glm::glm glfw KTX::ktx imgui::imgui)

# copy mono runtime DLL to the RTEngine runtime folder so the debugger/exe can find it
set(MONO_BIN_DIR "${CMAKE_SOURCE_DIR}/mono/bin")
set(MONO_SGEN_DLL_PATH "${MONO_BIN_DIR}/mono-2.0-sgen.dll")
set(MONO_LIB_DIR "${CMAKE_SOURCE_DIR}/mono/lib")

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders")

# Copy mono runtime DLL to the RTEngine runtime folder during configuration
if(EXISTS "${MONO_SGEN_DLL_PATH}")
    file(COPY "${MONO_SGEN_DLL_PATH}" DESTINATION "${CMAKE_BINARY_DIR}/RTEngine")
    message(STATUS "Copied mono/lib dir to runtime output directory during configuration.")
else()
    message(WARNING "mono-2.0-sgen.dll not found at ${MONO_SGEN_DLL_PATH}. Ensure it is available for runtime.")
endif()

# Copy assets directory to the RTEngine runtime folder during configuration
if(EXISTS "${ASSETS_DIR}")
    file(COPY "${ASSETS_DIR}" DESTINATION "${CMAKE_BINARY_DIR}/RTEngine/")
    message(STATUS "Copied assets directory to runtime output directory during configuration.")
else()
    message(WARNING "assets directory not found at ${ASSETS_DIR}.")
endif()

# Copy shaders directory to the RTEngine runtime folder during configuration
if(EXISTS "${SHADERS_DIR}")
    file(COPY "${SHADERS_DIR}" DESTINATION "${CMAKE_BINARY_DIR}/RTEngine/")
    message(STATUS "Copied shaders directory to runtime output directory during configuration.")
else()
    message(WARNING "assets directory not found at ${SHADERS_DIR}.")
endif()

# Copy mono/lib directory to the RTEngine runtime folder during configuration
if(EXISTS "${MONO_LIB_DIR}")
    file(COPY "${MONO_LIB_DIR}" DESTINATION "${CMAKE_BINARY_DIR}/RTEngine/mono")
    message(STATUS "Copied mono/lib directory to runtime output directory during configuration.")
else()
    message(WARNING "mono/lib directory not found at ${MONO_LIB_DIR}. Ensure it is available for runtime.")
endif()

set_target_properties(RTEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/RTEngine")
