project(RoarSandbox NONE)

set(CSPROJ_NAME "RoarSandbox.csproj")

add_custom_target(RoarSandbox_Build ALL
    COMMAND ${CMAKE_COMMAND} -E env LIB="" dotnet build "${CSPROJ_NAME}" -c $<CONFIG> -o "${COMMON_OUTPUT_DIR}/$<CONFIG>"
    
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Compiling RoarSandbox C#..."
    VERBATIM
)

add_dependencies(RoarSandbox_Build RoarScriptCore_Build)
