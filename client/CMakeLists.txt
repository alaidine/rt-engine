# Client executable
add_executable(${CLIENT}
    client.cpp
    client.h
)

# REUSE pch.h from core library
target_precompile_headers(${CLIENT} REUSE_FROM ${CORE_LIB})

# Link with core library (provides all shared dependencies)
target_link_libraries(${CLIENT} PRIVATE ${CORE_LIB})

# Include current directory for client-specific headers
target_include_directories(${CLIENT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directory
set_target_properties(${CLIENT} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# Visual Studio debugging setup
set_property(TARGET ${CLIENT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${CLIENT}>)

# Copy resources for client
add_custom_command(
    TARGET ${CLIENT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${CLIENT}>/resources
)
