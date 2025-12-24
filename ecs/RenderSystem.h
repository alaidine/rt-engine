#pragma once

#include "Component.h"
#include "ECS.h"
#include "Renderer.h"

class RenderSystem : public System {
  public:
    RenderSystem();
    ~RenderSystem();

    void Init(std::shared_ptr<rt::VulkanRenderer> renderer);
    void Update(Scene &scene, float dt);

  private:
    std::shared_ptr<rt::VulkanRenderer> mRenderer;
};
