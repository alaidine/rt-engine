#include "Application.h"

#include <glm/glm.hpp>

#include <assert.h>
#include <iostream>
#include <ranges>

namespace rt {

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
}

Application::Application(const ApplicationSpecification &specification, HINSTANCE hInstance) : mSpecification(specification) {
    sApplication = this;
    if (mSpecification.WindowSpec.Title.empty())
        mSpecification.WindowSpec.Title = mSpecification.Title;
    if (mSpecification.WindowSpec.Name.empty())
        mSpecification.WindowSpec.Name = mSpecification.Name;
    mSpecification.WindowSpec.EventCallback = [this](Event &event) { RaiseEvent(event); };
    mWindow = std::make_shared<Window>(mSpecification.WindowSpec);
    mWindow->Create(hInstance);

    mRenderer = std::make_shared<rt::VulkanRenderer>();
    mRenderer->InitWindowInfo(GetWindow()->window, hInstance, mSpecification.WindowSpec.Width, mSpecification.WindowSpec.Height);
    mRenderer->initVulkan();
    mRenderer->prepare();
    mRenderer->Init();
}

Application::~Application() {
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

        // Start frame drawing (clears previous frame buffers)
        mRenderer->StartDrawing();

        // Main layer update here
        for (const std::unique_ptr<Layer> &layer : m_LayerStack)
            layer->OnUpdate(timestep);

        // NOTE: rendering can be done elsewhere (eg. render thread)
        for (const std::unique_ptr<Layer> &layer : m_LayerStack)
            layer->OnRender();
        mRenderer->EndDrawing();

        // m_Window->Update();
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

float Application::GetTime() { return (float)0; }

} // namespace rt
