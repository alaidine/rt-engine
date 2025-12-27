add_library(core
    framework.h
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

target_include_directories(core PRIVATE ${MONO_INCLUDE_DIR})
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(core PRIVATE ecs raylib boost::boost glm::glm imgui::imgui spdlog::spdlog)
target_link_libraries(core PRIVATE "${MONO_LIB_DIR}/mono-2.0-sgen.lib")

target_include_directories(RTEngine PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(RTEngine PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(RTEngine PRIVATE core raylib boost::boost glm::glm imgui::imgui)

if (MSVC)
    target_link_options(RTEngine PRIVATE /FORCE:MULTIPLE)
    set_target_properties(RTEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:RTEngine>")
endif()


if(WIN32)
    set(LOCAL_MONO_DIR "$<TARGET_FILE_DIR:RTEngine>/mono")

    file(WRITE "${CMAKE_BINARY_DIR}/copy_mono_lazy.cmake" "
        # Check if the destination folder exists
        if(NOT IS_DIRECTORY \"\${DST}\")
            message(STATUS \"[RTEngine] Mono directory missing. Copying...\")
            
            # We use execute_process to run the copy command safely
            execute_process(COMMAND \${CMAKE_COMMAND} -E copy_directory \"\${SRC}\" \"\${DST}\")
        else()
            message(STATUS \"[RTEngine] Mono directory exists. Skipping copy.\")
        endif()
    ")

    add_custom_command(TARGET RTEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} 
            -DSRC="${MONO_ROOT}"
            -DDST="LOCAL_MONO_DIR"
            -P "${CMAKE_BINARY_DIR}/copy_mono_lazy.cmake"
        COMMENT "Checking Mono Runtime..."
    )

    add_custom_command(TARGET RTEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MONO_BIN_DIR}/mono-2.0-sgen.dll"
        "$<TARGET_FILE_DIR:RTEngine>/mono-2.0-sgen.dll"
        COMMENT "Copying Mono Runtime to output directory..."
    )
endif()

target_include_directories(CoreTests PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(CoreTests PRIVATE core raylib boost::boost glm::glm imgui::imgui)

add_test(NAME TestCore
    COMMAND ${CMAKE_BINARY_DIR}/core/CoreTests)

