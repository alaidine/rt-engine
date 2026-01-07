add_library(RoarEngine
        Common.h
        framework.h
        Event.h
        Entrypoint.h
        InputEvents.h
        Window.cpp
        Window.h
        WindowEvents.h
        Application.cpp
        Application.h
        Layer.cpp
        Layer.h)

target_include_directories(RoarEngine PRIVATE ${CMAKE_SOURCE_DIR}/renderer/)
target_link_libraries(RoarEngine PUBLIC networking renderer raylib spdlog::spdlog glm::glm)
