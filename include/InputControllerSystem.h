#pragma once
    #include "Scene.h"
    #include "System.h"
    #include "Component.h"
    #include "Prefab.h"

    #ifndef MAX
    #define MAX(a, b) (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef MIN
    #define MIN(a, b) (((a) < (b)) ? (a) : (b))
    #endif

    #define GAME_WIDTH 800
    #define GAME_HEIGHT 600

    #define TARGET_FPS 100

extern Scene _core;

void fire(Vector2& missilePos, Vector2& palyerPos, AnimationComponent& missileAnim)
{
    missilePos.x = palyerPos.x;
    missilePos.y = palyerPos.y;
    missileAnim.rect.x = missileAnim._animationRectangle[missileAnim._current_frame].x;
    missileAnim.rect.y = missileAnim._animationRectangle[missileAnim._current_frame].y;
    missileAnim.rect.height = missileAnim._animationRectangle[missileAnim._current_frame].height;
    missileAnim.rect.width = missileAnim._animationRectangle[missileAnim._current_frame].width;
}

class InputControllerSystem : public System {
    public:
        void Update() override {
            for (auto& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& input = _core.GetComponent<InputController>(entity);
                auto& playertexture = _core.GetComponent<PlayerSprite>(entity);
                auto& cooldown = _core.GetComponent<playerCooldown>(entity);
                auto& velocity = _core.GetComponent<Velocity>(entity);
                velocity.speedX = 0;
                velocity.speedY = 0;
                if (IsKeyDown(KEY_SPACE) && !cooldown.canFire) {
                    cooldown.canFire = true;
                    Entity missile = Prefab::MakeMilssile(_core);
                    auto& missilePos = _core.GetComponent<Position>(missile);
                    auto&  anim = _core.GetComponent<AnimationComponent>(missile);
                    auto& sprite = _core.GetComponent<Sprite>(missile);
                    fire(missilePos.position, pos.position, anim);
                }
                if (IsKeyUp(KEY_SPACE))
                    cooldown.canFire = false;
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
                    velocity.speedY -= 5.0f;
                else if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
                    velocity.speedY += 5.0f;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                    velocity.speedX -= 5.0f; 
                else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                    velocity.speedX += 5.0f;
            }
        }
};
