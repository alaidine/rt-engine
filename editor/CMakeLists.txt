add_executable(RoarEditor
    Editor.cpp)

target_include_directories(RoarEditor PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(RoarEditor PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_include_directories(RoarEditor PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_link_libraries(RoarEditor PRIVATE core renderer raylib glm::glm)

set_target_properties(RoarEditor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:RoarEditor>")

