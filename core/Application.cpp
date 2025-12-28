#include "Application.h"
#include "Scripting.h"

#include <glm/glm.hpp>

#include <assert.h>
#include <iostream>
#include <ranges>

namespace Roar {

static Application *sApplication = nullptr;

static void GLFWErrorCallback(int error, const char *description) { std::cerr << "[GLFW Error]: " << description << std::endl; }

Application::Application(const ApplicationSpecification &specification) : mSpecification(specification) {
    sApplication = this;
    if (mSpecification.WindowSpec.Title.empty())
        mSpecification.WindowSpec.Title = mSpecification.Title;
    if (mSpecification.WindowSpec.Name.empty())
        mSpecification.WindowSpec.Name = mSpecification.Name;
    mSpecification.WindowSpec.EventCallback = [this](Event &event) { RaiseEvent(event); };
    mWindow = std::make_shared<Window>(mSpecification.WindowSpec);
    mWindow->Create();

    Scripting::Init();
}

Application::~Application() {
    Scripting::Shutdown();
    mWindow->Destroy();
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

        // Start frame drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
        // Main layer update here
        for (const std::unique_ptr<Layer> &layer : m_LayerStack)
            layer->OnUpdate(timestep);

        // NOTE: rendering can be done elsewhere (eg. render thread)
        for (const std::unique_ptr<Layer> &layer : m_LayerStack)
            layer->OnRender();
        EndDrawing();
    }
}

void Application::Stop() { m_Running = false; }

void Application::RaiseEvent(Event &event) {
    for (auto &layer : std::views::reverse(m_LayerStack)) {
        layer->OnEvent(event);
        if (event.Handled)
            break;
    }
}

// glm::vec2 Application::GetFramebufferSize() const { return m_Window->GetFramebufferSize(); }
glm::vec2 Application::GetFramebufferSize() const { return glm::vec2(0); }

Application &Application::Get() {
    assert(sApplication);
    return *sApplication;
}

} // namespace Roar
