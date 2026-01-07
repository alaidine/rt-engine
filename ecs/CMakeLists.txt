add_library(ecs
    ECS.h
    Component.h
    GuiSystem.h
    GuiSystem.cpp
    ServerSystem.h
    ServerSystem.cpp
    ClientSystem.h
    ClientSystem.cpp
    TextureSystem.h
    TextureSystem.cpp
    RenderSystem.cpp
    RenderSystem.h)

target_include_directories(ecs PRIVATE ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(ecs PRIVATE ${CMAKE_SOURCE_DIR}/core)
target_include_directories(ecs PRIVATE ${CMAKE_SOURCE_DIR}/ecs)
target_include_directories(ecs PRIVATE ${MONO_INCLUDE_DIR})
target_link_libraries(ecs PRIVATE renderer core raylib asio glm::glm spdlog::spdlog)
