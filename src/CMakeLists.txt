add_library(RoarEngine
    ../include/RoarEngine.h
    ../include/Common.h
    ../include/Event.h
    ../include/Entrypoint.h
    ../include/InputEvents.h
    Window.cpp
    ../include/Window.h
    ../include/WindowEvents.h
    Application.cpp
    Layer.cpp
    PluginManager.cpp
    RoarEngine.cpp
    ../include/PluginManager.h)
target_include_directories(RoarEngine PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RoarEngine PUBLIC raylib spdlog::spdlog glm::glm)

# ECS plugin
add_library(ecs SHARED
    ../include/ECS.h
    ../include/Component.h
    ../include/GuiSystem.h
    GuiSystem.cpp
    ../include/ServerSystem.h
    ServerSystem.cpp
    ../include/ClientSystem.h
    ClientSystem.cpp
    ../include/TextureSystem.h
    TextureSystem.cpp
    RenderSystem.cpp
    ../include/RenderSystem.h)
target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ecs PUBLIC raylib asio::asio glm::glm spdlog::spdlog)

# Networking plugin
add_library(networking SHARED Networking.cpp ../include/Networking.h)
target_link_libraries(networking PUBLIC asio::asio)

# Test plugin
add_library(TestPlugin SHARED TestPlugin.cpp)
target_include_directories(TestPlugin PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Test game
add_executable(TestGame TestGame.cpp)
target_link_libraries(TestGame RoarEngine)

# pong
add_executable(pong pong.cpp)
target_link_libraries(TestGame RoarEngine)

# R-Type client
add_executable(r-type_client r-type_client.cpp)
target_link_libraries(TestGame RoarEngine)

# R-Type server
add_executable(r-type_server r-type_server.cpp)
target_link_libraries(TestGame RoarEngine)
