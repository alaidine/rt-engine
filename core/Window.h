#pragma once

#include "Event.h"

#include <glm/glm.hpp>

#include <functional>
#include <string>

#include "framework.h"
#include "raylib.h"

namespace Roar {

struct WindowSpecification {
    std::string Title;
    std::string Name;
    uint32_t Width = 1280;
    uint32_t Height = 720;
    bool IsResizeable = true;
    bool VSync = true;

    using EventCallbackFn = std::function<void(Event &)>;
    EventCallbackFn EventCallback;
};

class Window {
  public:
    Window(const WindowSpecification &specification = WindowSpecification());
    ~Window();

    void Create();
    void Destroy();

    void Update();

    void RaiseEvent(Event &event);
    void PollEvent();

    glm::vec2 GetFramebufferSize() const;
    glm::vec2 GetMousePos() const;

    bool ShouldClose() const;

    struct Settings {
        bool validation = false;
        bool fullscreen = false;
        bool vsync = false;
        bool overlay = true;
    } settings;

    bool quitMessageReceived = false;

  private:
    WindowSpecification mSpecification;
    std::string getWindowTitle() const;

    // Frame counter to display fps
    uint32_t mFrameCounter = 0;
    uint32_t mLastFPS = 0;
};

} // namespace Roar
