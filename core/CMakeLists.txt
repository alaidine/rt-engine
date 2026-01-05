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
        InputEvents.h
        Window.cpp
        Window.h
        WindowEvents.h
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
    target_link_libraries(nfd PRIVATE gtk::gtk)
endif ()


target_include_directories(core PRIVATE ${MONO_INCLUDE_DIR})
target_include_directories(core PRIVATE ${NFD_INCLUDE})
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(core PRIVATE renderer ecs nfd raylib glm::glm imgui::imgui spdlog::spdlog)
target_link_libraries(core PRIVATE "${MONO_LINK_LIB}")

target_include_directories(RoarEngine PRIVATE ${NFD_INCLUDE})
target_include_directories(RoarEngine PRIVATE ${MONO_INCLUDE_DIR})
target_include_directories(RoarEngine PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(RoarEngine PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_include_directories(RoarEngine PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(RoarEngine PRIVATE renderer core nfd raylib glm::glm imgui::imgui spdlog::spdlog)
target_link_libraries(RoarEngine PRIVATE "${MONO_LINK_LIB}")
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

set(MONO_SRC_DIR "${MONO_ROOT}")
set(MONO_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/mono")

set(MONO_SENTINEL "${MONO_DEST_DIR}/lib/mono/4.5/mscorlib.dll")

add_custom_command(
    OUTPUT "${MONO_SENTINEL}"
    
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${MONO_SRC_DIR}/common/lib/4.5"
    "$<TARGET_FILE_DIR:RoarEngine>/mono/lib/mono/4.5"

    
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${MONO_SRC_DIR}/common/etc"
    "$<TARGET_FILE_DIR:RoarEngine>/mono/etc"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${MONO_SRC_DIR}/common/include"
    "$<TARGET_FILE_DIR:RoarEngine>/mono/include"

    
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MONO_RUNTIME_LIB}" 
    "$<TARGET_FILE_DIR:RoarEngine>"
    
    COMMENT "Initializing Mono Dependencies (Run Once)..."
    VERBATIM
)

add_custom_target(EnsureMonoDependencies DEPENDS "${MONO_SENTINEL}")
add_dependencies(RoarEngine EnsureMonoDependencies)
