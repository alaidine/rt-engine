#pragma once
#include "Scene.h"
#include "System.h"
#include <raylib.h>

class ClientRendererSystem : public System {
private:
    Texture2D* m_playerTexture;
    Rectangle sourcerec;
    Rectangle destrec;
    Rectangle rec;
    Vector2 origin = { 0.0f, 0.0f };
public:
    void SetPlayerTexture(Texture2D* texture) {
        m_playerTexture = texture;
    }

    void RenderClient(Scene& core, Entity entity) {
        auto& pos = core.GetComponent<Position>(entity);
        auto& anim = core.GetComponent<AnimationComponent>(entity);
        auto& sprite = core.GetComponent<Sprite>(entity);
        auto& netClient = core.GetComponent<NetworkedClient>(entity);

        float frameWidth = 32;
        float frameHeight = 22.0f;
        sourcerec = { 0.0f, 30.0f, frameWidth, frameHeight };
        rec = { pos.position.x, pos.position.y, frameWidth * 2.0f, frameHeight * 2.0f };
        destrec = { pos.position.x, pos.position.y, frameWidth * 2.0f, frameHeight * 2.0f };

        DrawTexturePro(*m_playerTexture, sourcerec, destrec, origin, 0.0f, sprite.color);

        // Draw rectangle around local player
        if (netClient.is_local) {
            DrawRectangleLinesEx(rec, 3, DARKBROWN);
        }
    }

    void Update() override {
        // This system requires manual update with Core reference
    }
};
