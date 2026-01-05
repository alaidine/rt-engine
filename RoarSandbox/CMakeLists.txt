project(RoarSandbox NONE)

set(CSPROJ_NAME "RoarSandbox.csproj")

if (WIN32)
add_custom_target(RoarSandbox_Build ALL
    COMMAND ${CMAKE_COMMAND} -E env LIB="" dotnet build "${CSPROJ_NAME}" -c $<CONFIG> -o "${COMMON_OUTPUT_DIR}/$<CONFIG>"
    
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Compiling RoarSandbox C#..."
    VERBATIM
)
elseif(UNIX AND NOT APPLE)
add_custom_target(RoarSandbox_Build ALL
    COMMAND ${CMAKE_COMMAND} -E env LIB="" dotnet build "${CSPROJ_NAME}" -c $<CONFIG> -o "${COMMON_OUTPUT_DIR}"
    
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Compiling RoarSandbox C#..."
    VERBATIM
)
endif()

add_dependencies(RoarSandbox_Build RoarScriptCore_Build)
