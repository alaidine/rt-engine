#include "Application.h"
#include "Common.h"

#include "raylib.h"

#include <glm/glm.hpp>

#include <assert.h>
#include <iostream>
#include <ranges>

extern bool g_ApplicationRunning;

namespace Roar {

static Application *sApplication = nullptr;

static void GLFWErrorCallback(int error, const char *description) { std::cerr << "[GLFW Error]: " << description << std::endl; }

void CustomTraceLog(int msgType, const char *text, va_list args) {
    // Calculate the length of the final string
    // We must copy args because vsnprintf "consumes" the list
    va_list args_copy;
    va_copy(args_copy, args);
    int len = std::vsnprintf(nullptr, 0, text, args_copy);
    va_end(args_copy);

    if (len < 0) return; // Encoding error

    // Allocate a buffer to hold the formatted string
    // (len + 1 for the null terminator)
    std::vector<char> buffer(len + 1);

    // Format the string into the buffer
    std::vsnprintf(buffer.data(), buffer.size(), text, args);

    // Log the resolved string
    // We use "{}", buffer.data() to be safe in case the message contains brace characters
    switch (msgType) {
        case LOG_INFO:    spdlog::info("{}", buffer.data()); break;
        case LOG_ERROR:   spdlog::error("{}", buffer.data()); break;
        case LOG_WARNING: spdlog::warn("{}", buffer.data()); break;
        case LOG_DEBUG:   spdlog::debug("{}", buffer.data()); break;
        default: break;
    }
}

Application::Application(const ApplicationSpecification &specification) : mSpecification(specification) {
    sApplication = this;
    if (mSpecification.WindowSpec.Title.empty())
        mSpecification.WindowSpec.Title = mSpecification.Title;
    if (mSpecification.WindowSpec.Name.empty())
        mSpecification.WindowSpec.Name = mSpecification.Name;
    mSpecification.WindowSpec.EventCallback = [this](Event &event) { RaiseEvent(event); };

    SetTraceLogCallback(CustomTraceLog);

    mWindow = std::make_shared<Window>(mSpecification.WindowSpec);
    mWindow->Create();
}

Application::~Application() {
    g_ApplicationRunning = false;
    rlImGuiShutdown();
    sApplication = nullptr;
}

void Application::Run() {
    m_Running = true;

    float lastTime = GetTime();

    // Main Application loop
    while (m_Running) {
        mWindow->PollEvent();

        if (mWindow->ShouldClose()) {
            Stop();
            break;
        }

        float currentTime = GetTime();
        float timestep = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
        lastTime = currentTime;

        // Main layer update here
        for (const std::unique_ptr<Layer> &layer : m_LayerStack)
            layer->OnUpdate(timestep);

        // NOTE: rendering can be done elsewhere (eg. render thread)
        for (const std::unique_ptr<Layer> &layer : m_LayerStack)
            layer->OnRender();

        // Apply any queued transitions after all layers have finished their
        // OnUpdate/OnRender for this frame. This avoids destroying a layer
        // while one of its member functions is still executing.
        if (!m_QueuedTransitions.empty()) {
            for (auto &entry : m_QueuedTransitions) {
                Layer *from = entry.first;
                std::unique_ptr<Layer> &newLayer = entry.second;
                for (auto &slot : m_LayerStack) {
                    if (slot.get() == from) {
                        slot = std::move(newLayer);
                        break;
                    }
                }
            }
            m_QueuedTransitions.clear();
        }
    }
}

void Application::QueueTransition(std::unique_ptr<Layer> layer, Layer *from) {
    m_QueuedTransitions.emplace_back(from, std::move(layer));
}

void Application::Stop() { m_Running = false; }

void Application::RaiseEvent(Event &event) {
    for (auto &layer : std::views::reverse(m_LayerStack)) {
        layer->OnEvent(event);
        if (event.Handled)
            break;
    }
}

Application &Application::Get() {
    assert(sApplication);
    return *sApplication;
}

} // namespace Roar
