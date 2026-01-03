#include "Window.h"

#include "InputEvents.h"
#include "WindowEvents.h"

#include <assert.h>
#include <iostream>

namespace Roar {

Window::Window(const WindowSpecification &specification) : mSpecification(specification) {}

Window::~Window() { Destroy(); }

void Window::PollEvent() {}

void Window::Create() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(mSpecification.Width, mSpecification.Height, mSpecification.Name.c_str());
    SetTargetFPS(60);
}

std::string Window::getWindowTitle() const {
    std::string windowTitle{mSpecification.Name};
    if (!settings.overlay) {
        windowTitle += " - " + std::to_string(mFrameCounter) + " fps";
    }
    return windowTitle;
}

void Window::Destroy() { CloseWindow(); }

void Window::Update() {}

void Window::RaiseEvent(Event &event) {
    if (mSpecification.EventCallback)
        mSpecification.EventCallback(event);
}

glm::vec2 Window::GetFramebufferSize() const {
    int width = 0;
    int height = 0;
    return {width, height};
}

glm::vec2 Window::GetMousePos() const {
    double x = 0;
    double y = 0;
    return {static_cast<float>(x), static_cast<float>(y)};
}

bool Window::ShouldClose() const { return WindowShouldClose(); }

} // namespace Roar
