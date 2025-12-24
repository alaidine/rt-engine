#include "RenderSystem.h"

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::Init(std::shared_ptr<rt::VulkanRenderer> renderer) {
    mRenderer = renderer;
}

void RenderSystem::Update(Scene& scene, float dt) {
    for (auto const &entity : mEntities) {
        auto &rectangleShape = scene.GetComponent<RectangleShape>(entity);
        auto &transform = scene.GetComponent<Transform2D>(entity);

        mRenderer->DrawRect({transform.pos[0], transform.pos[1], transform.size[0], transform.size[1]},
                            {rectangleShape.color[0], rectangleShape.color[1], rectangleShape.color[2]});
    }
}
