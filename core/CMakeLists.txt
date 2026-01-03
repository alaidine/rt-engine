add_library(core
        ProjectManager.h
        ProjectManager.cpp
        Common.h
        EditorLayer.h
        EditorLayer.cpp
        SceneLayer.h
        SceneLayer.cpp
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
        RoarConfig.h)

add_executable(RoarEngine
        RoarEngine.cpp
        RoarEngine.h)

if (WIN32) # Windows
    add_library(nfd
            ${NFD_SRC}/include/nfd.h
            ${NFD_SRC}/common.h
            ${NFD_SRC}/nfd_common.c
            ${NFD_SRC}/nfd_common.h
            ${NFD_SRC}/nfd_win.cpp
            ${NFD_SRC}/simple_exec.h
    )
    target_include_directories(nfd PRIVATE ${NFD_INCLUDE})
elseif (UNIX AND NOT APPLE) # Linux
    add_library(nfd
            ${NFD_SRC}/include/nfd.h
            ${NFD_SRC}/common.h
            ${NFD_SRC}/nfd_common.c
            ${NFD_SRC}/nfd_common.h
            ${NFD_SRC}/nfd_gtk.c
            ${NFD_SRC}/simple_exec.h
    )
    target_include_directories(nfd PRIVATE ${NFD_INCLUDE})
endif ()


target_include_directories(core PRIVATE ${MONO_INCLUDE_DIR})
target_include_directories(core PRIVATE ${NFD_INCLUDE})
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(core PRIVATE renderer ecs nfd raylib glm::glm imgui::imgui spdlog::spdlog)
target_link_libraries(core PRIVATE "${MONO_LIB_DIR}/mono-2.0-sgen.lib")

target_include_directories(RoarEngine PRIVATE ${NFD_INCLUDE})
target_include_directories(RoarEngine PRIVATE ${MONO_INCLUDE_DIR})
target_include_directories(RoarEngine PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(RoarEngine PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(RoarEngine PRIVATE renderer core nfd raylib glm::glm imgui::imgui spdlog::spdlog)
target_link_libraries(RoarEngine PRIVATE "${MONO_LIB_DIR}/mono-2.0-sgen.lib")
set_target_properties(RoarEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:RoarEngine>")

if (MSVC)
    target_link_options(RoarEngine PRIVATE /FORCE:MULTIPLE)
endif ()

if(UNIX AND NOT APPLE)
    set_target_properties(RoarEngine PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "$ORIGIN"
            LINK_FLAGS "-Wl,-rpath,'$ORIGIN'"
    )
endif()

add_custom_command(TARGET RoarEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MONO_RUNTIME_LIB}"
        "$<TARGET_FILE_DIR:RoarEngine>"
        COMMENT "Copying Mono Runtime to output directory..."
)

# Copy Common Configs (mscorlib, etc.)
add_custom_command(TARGET MyEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${MONO_PATH}/Common/lib"
        "$<TARGET_FILE_DIR:MyEngine>/mono/lib"
        COMMENT "Copying Mono Class Libraries..."
)

add_custom_command(TARGET MyEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${MONO_ROOT}/common/etc"
        "$<TARGET_FILE_DIR:MyEngine>/mono/etc"
        COMMENT "Copying Mono Config..."
)
