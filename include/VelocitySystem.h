#pragma once
#include "Scene.h"
#include "System.h"
#include "raylib.h"

extern Scene _core;

class VelocitySystem : public System {
  public:
    void Update() override {

        for (auto const &entity : _entities) {
            auto &pos = _core.GetComponent<Position>(entity);
            auto &velocity = _core.GetComponent<Velocity>(entity);
            pos.position.x += velocity.speedX;
            pos.position.y += velocity.speedY;
        }
    }
};
