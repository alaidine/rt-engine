add_library(RoarEngine SHARED
    ../include/framework.h
    ../include/RoarEngine.h
    ../include/Common.h
    PluginManager.cpp
    RoarEngine.cpp
    ../include/PluginManager.h)
target_include_directories(RoarEngine PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RoarEngine PUBLIC raylib spdlog::spdlog glm::glm)

target_compile_definitions(RoarEngine PRIVATE ENGINE_EXPORTS)

if(WIN32)
    add_custom_command(TARGET RoarEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:RoarEngine>
            $<TARGET_FILE_DIR:RoarEngine>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying dependency DLLs to output directory..."
    )
endif()

# ECS plugin
add_library(ECSPlugin SHARED
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
target_include_directories(ECSPlugin PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ECSPlugin PUBLIC raylib asio::asio glm::glm spdlog::spdlog)

# Networking plugin
add_library(NetworkPlugin SHARED Networking.cpp ../include/INetwork.h ../include/Networking.h)
target_include_directories(NetworkPlugin PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(NetworkPlugin PUBLIC RoarEngine)
if(WIN32)
    target_link_libraries(NetworkPlugin PUBLIC ws2_32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(NetworkPlugin PUBLIC pthread)
endif()

# Test plugin
add_library(TestPlugin SHARED TestPlugin.cpp)
target_include_directories(TestPlugin PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(TestPlugin PUBLIC RoarEngine)

# Box2D Physics plugin
add_library(Box2DPhysicsPlugin SHARED Box2DPhysics.cpp ../include/IPhysics.h ../include/Box2DPhysics.h)
target_include_directories(Box2DPhysicsPlugin PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Box2DPhysicsPlugin PUBLIC RoarEngine box2d::box2d)

if(WIN32)
    add_custom_command(TARGET Box2DPhysicsPlugin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:Box2DPhysicsPlugin>
            $<TARGET_FILE_DIR:Box2DPhysicsPlugin>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying dependency DLLs to output directory..."
    )
endif()

# Audio plugin
add_library(AudioPlugin SHARED AudioPlugin.cpp ../include/IAudio.h)
target_include_directories(AudioPlugin PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(AudioPlugin PUBLIC RoarEngine)

# Test game
add_executable(TestGame TestGame.cpp)
target_include_directories(TestGame PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(TestGame RoarEngine)

# Pong
add_executable(Pong pong.cpp)
target_include_directories(Pong PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Pong RoarEngine)

# Platformer
add_executable(Platformer Platformer.cpp)
target_include_directories(Platformer PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Platformer RoarEngine)

# R-Type client
add_executable(R-Type_Client r-type_client.cpp r-type.cpp ${CMAKE_SOURCE_DIR}/include/r-type.h)
target_include_directories(R-Type_Client PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(R-Type_Client RoarEngine)

# R-Type server
add_executable(R-Type_Server r-type_server.cpp r-type.cpp ${CMAKE_SOURCE_DIR}/include/r-type.h)
target_include_directories(R-Type_Server PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(R-Type_Server RoarEngine)
