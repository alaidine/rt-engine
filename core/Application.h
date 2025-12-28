#pragma once

#include "Event.h"
#include "Layer.h"
#include "Window.h"

#include "Renderer.h"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Roar {

struct ApplicationSpecification {
    std::string Name = "Application";
    std::string Title = "app";
    WindowSpecification WindowSpec;
};

class Application {
  public:
    Application(const ApplicationSpecification &specification = ApplicationSpecification());

    ~Application();

    void Run();
    void Stop();

    void RaiseEvent(Event &event);

    template <typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
    void PushLayer() {
        m_LayerStack.push_back(std::make_unique<TLayer>());
    }

    template <typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
    TLayer *GetLayer() {
        for (const auto &layer : m_LayerStack) {
            if (auto casted = dynamic_cast<TLayer *>(layer.get()))
                return casted;
        }
        return nullptr;
    }

    glm::vec2 GetFramebufferSize() const;

    std::shared_ptr<Window> GetWindow() const { return mWindow; }

    static Application &Get();
    static float GetTime();

  private:
    ApplicationSpecification mSpecification;
    std::shared_ptr<Window> mWindow;
    bool m_Running = false;
    std::vector<std::unique_ptr<Layer>> m_LayerStack;
    friend class Layer;
};

} // namespace Roar
