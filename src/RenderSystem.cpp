#include "RenderSystem.h"
#include "framework.h"
#include "raylib.h"

namespace Roar {

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::Init() {}

void RenderSystem::Update(Scene &scene) {
    for (auto const &entity : mEntities) {
        auto &rectangle = scene.GetComponent<RectangleComponent>(entity);
        auto &transform = scene.GetComponent<TransformComponent>(entity);

        DrawRectangleV(Vector2{transform.pos.x, transform.pos.y}, Vector2{transform.size.x, transform.size.y},
                       Color{rectangle.color[0], rectangle.color[1], rectangle.color[2], rectangle.color[3]});
    }
}

} // namespace Roar
