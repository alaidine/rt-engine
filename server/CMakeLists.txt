# Server executable
add_executable(${SERVER}
    server.cpp
    server.h
)

# REUSE pch.h from core library
target_precompile_headers(${CLIENT} REUSE_FROM ${CORE_LIB})

# Link with core library (provides all shared dependencies)
target_link_libraries(${SERVER} PRIVATE ${CORE_LIB})

# Include current directory for server-specific headers
target_include_directories(${SERVER} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directory
set_target_properties(${SERVER} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# Visual Studio debugging setup
set_property(TARGET ${SERVER} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${SERVER}>)
