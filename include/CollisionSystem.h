#pragma once

#include "Scene.h"
#include "System.h"

#include "framework.h"
#include <raylib.h>

extern Scene _core;

void check_map_collision(Position &pos, Collider &collider) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    if (pos.position.x + collider.rect.width >= screenW)
        pos.position.x = screenW - collider.rect.width;

    if (pos.position.x <= 0)
        pos.position.x = 0;

    if (pos.position.y + collider.rect.height >= screenH)
        pos.position.y = screenH - collider.rect.height;

    if (pos.position.y <= 0)
        pos.position.y = 0;

    collider.rect.x = pos.position.x;
    collider.rect.y = pos.position.y;
}

void check_enemy_collision(Rectangle &rectA, Rectangle &rectB) {
    bool collision;
    collision = CheckCollisionRecs(rectA, rectB);
    if (collision) {
        std::cout << "collision with enemy" << std::endl;
        collision = false;
    }
}

class CollisionSystem : public System {
  public:
    void Update() override {
        bool collision;
        Rectangle rectA;
        Rectangle rectB;
        for (auto &entity : _entities) {
            auto &pos = _core.GetComponent<Position>(entity);
            auto &collider = _core.GetComponent<Collider>(entity);
            if (collider.isPlayer) {
                check_map_collision(pos, collider);
                rectA = collider.rect;
            } else if (!collider.isPlayer)
                rectB = collider.rect;
            // check enemy collision
            check_enemy_collision(rectA, rectB);
        }
    }
};
