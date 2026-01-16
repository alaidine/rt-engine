#pragma once
    #include "Scene.h"
    #include "System.h"
    #define TARGET_FPS 100
    #define GAME_WIDTH 800

extern Scene _core;

void AnimMissile(Position& pos, AnimationComponent& anim) {
    anim._frameCounter++;

    if (anim._frameCounter >= (TARGET_FPS / anim._frameSpeed))
    {
        anim._frameCounter = 0;
        anim._current_frame++;
        if (anim._current_frame > 4) anim._current_frame = 0;
        anim.rect.x = anim._animationRectangle[anim._current_frame].x;
        anim.rect.y = anim._animationRectangle[anim._current_frame].y;
        anim.rect.height = anim._animationRectangle[anim._current_frame].height;
        anim.rect.width = anim._animationRectangle[anim._current_frame].width;
    }

    pos.position.x += 5;
}


class MissileSystem : public System {
    public:
        void Update() override {
            for (auto& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& anim = _core.GetComponent<AnimationComponent>(entity);
                auto& tag = _core.GetComponent<MissileTag>(entity);
                AnimMissile(pos, anim);
                if (pos.position.x > GAME_WIDTH) {
                    std::cout << "Destroying missile at x=" << pos.position.x << std::endl;
                    _core.DestroyEntity(entity);
                    break;
                }
            }
        }
};
