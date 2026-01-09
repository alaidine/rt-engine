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

target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/renderer)
target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/core)
target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/ecs)
target_link_libraries(ecs PUBLIC renderer core raylib asio glm::glm spdlog::spdlog)
