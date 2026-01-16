#pragma once
#include "Scene.h"
#include <raylib.h>

extern Scene _core;

// class PlayerRender

class RendererSystem : public System {
  private:
    Texture2D texture;
    Rectangle sourcerec;
    Rectangle destrec;
    Rectangle rec;
    Vector2 origin = {0.0f, 0.0f};

  public:
    CameraComponent *GetMainCamera() {
        for (auto const &entity : _entities) {
            if (!_core.HasComponent<CameraComponent>(entity))
                continue;
            auto &cam = _core.GetComponent<CameraComponent>(entity);
            if (cam.mainCamera)
                return &cam;
        }
        return nullptr;
    };

    void Update() override {

        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        CameraComponent *cam = nullptr;
        DrawText("OUI oui", 10, 10, 20, BLACK);

        for (auto const &entity : _entities) {
            auto &pos = _core.GetComponent<Position>(entity);
            auto &sprite = _core.GetComponent<Sprite>(entity);
            auto &anim = _core.GetComponent<AnimationComponent>(entity);
            auto &tag = _core.GetComponent<Tag>(entity);

            if (_core.HasComponent<PlayerSprite>(entity)) {
                auto &playerTexture = _core.GetComponent<PlayerSprite>(entity);
                texture = playerTexture.texture;
            }

            sourcerec = {anim.rect.x, anim.rect.y, anim.rect.width, anim.rect.height};
            if (_core.HasComponent<CameraComponent>(entity))
                cam = GetMainCamera();

            Vector2 screenPos = pos.position;

            if (cam) {
                screenPos.x = cam->offset.x + (pos.position.x - cam->position.x) * cam->zoom;
                screenPos.y = cam->offset.y + (pos.position.y - cam->position.y) * cam->zoom;
            }

            float scale = cam ? cam->zoom : 1.0f;

            destrec = {screenPos.x, screenPos.y, anim.rect.width * 2.0f * scale, anim.rect.height * 2.0f * scale};

            DrawTexturePro(texture, sourcerec, destrec, origin, 0.0f, sprite.color);
        }

        EndDrawing();
    }
};
