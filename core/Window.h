#pragma once

#pragma comment(linker, "/subsystem:windows")
#include <ShellScalingApi.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#include <GLFW/glfw3.h>

#include "event.h"

#include <glm/glm.hpp>

#include <functional>
#include <string>

#include <keycodes.hpp>

namespace rt {

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

    HWND Create(HINSTANCE hinstance);
    void Create();
    void Destroy();

    void Update();

    void RaiseEvent(Event &event);
    void PollEvent();

    glm::vec2 GetFramebufferSize() const;
    glm::vec2 GetMousePos() const;

    bool ShouldClose() const;

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnHandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    GLFWwindow *GetHandle() const { return mHandle; }

    struct Settings {
        bool validation = false;
        bool fullscreen = false;
        bool vsync = false;
        bool overlay = true;
    } settings;

    HWND window;
    HINSTANCE windowInstance;

    MSG msg;
    bool quitMessageReceived = false;

  private:
    WindowSpecification mSpecification;
    GLFWwindow *mHandle = nullptr;

    std::string getWindowTitle() const;

    // Frame counter to display fps
    uint32_t mFrameCounter = 0;
    uint32_t mLastFPS = 0;
};

} // namespace rt
