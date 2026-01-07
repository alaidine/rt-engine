#include "Window.h"

#include "InputEvents.h"
#include "WindowEvents.h"

#include <assert.h>
#include <iostream>

namespace Roar {

Window::Window(const WindowSpecification &specification) : mSpecification(specification) {}

Window::~Window() { Destroy(); }

void Window::Create() {
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

glm::vec2 Window::GetMousePos() const {
    double x = 0;
    double y = 0;
    return {static_cast<float>(x), static_cast<float>(y)};
}

bool Window::ShouldClose() const { return WindowShouldClose(); }

} // namespace Roar
