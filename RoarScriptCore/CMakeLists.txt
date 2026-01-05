project(RoarScriptCore NONE)

set(CSPROJ_NAME "RoarScriptCore.csproj")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DOTNET_CONFIG "Release")
else()
    set(DOTNET_CONFIG "Debug")
endif()

file(MAKE_DIRECTORY "${COMMON_OUTPUT_DIR}")

if (WIN32)
add_custom_target(RoarScriptCore_Build ALL
    # WE USE "RoarScriptCore.csproj" (Relative Path)
    # AND WE SET WORKING_DIRECTORY to where that file is.
    COMMAND ${CMAKE_COMMAND} -E env LIB="" dotnet build "${CSPROJ_NAME}" -c $<CONFIG> -o "${COMMON_OUTPUT_DIR}/$<CONFIG>"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Compiling C# Core..."
    VERBATIM
)
elseif(UNIX AND NOT APPLE)
add_custom_target(RoarScriptCore_Build ALL
    # WE USE "RoarScriptCore.csproj" (Relative Path)
    # AND WE SET WORKING_DIRECTORY to where that file is.
    COMMAND ${CMAKE_COMMAND} -E env LIB="" dotnet build "${CSPROJ_NAME}" -c $<CONFIG> -o "${COMMON_OUTPUT_DIR}/"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Compiling C# Core..."
    VERBATIM
)
endif()
