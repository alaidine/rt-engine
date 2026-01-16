#pragma once

#include "Scene.h"
#include "System.h"

extern Scene _core;

class CameraSystem : public System {
  public:
    void Update() override {
        float followSpeed = 0.1f;

        for (auto entity : _entities) {
            if (!_core.HasComponent<CameraComponent>(entity))
                continue;

            auto &cam = _core.GetComponent<CameraComponent>(entity);

            Vector2 dif = {cam.target.x - cam.position.x, cam.target.y - cam.position.y};

            cam.position.x += dif.x * followSpeed;
            cam.position.y += dif.y * followSpeed;
        }
    }
};

class CameraFollowSystem : public System {
  public:
    void Update() override {
        Entity playerEntity = -1;

        // find local player
        for (auto entity : _entities) {
            if (_core.HasComponent<Position>(entity) && _core.HasComponent<LocalPlayerTag>(entity)) {
                playerEntity = entity;
                break;
            }
        }

        if (playerEntity == -1)
            return;

        auto &playerPos = _core.GetComponent<Position>(playerEntity);

        // update camera target
        for (auto entity : _entities) {
            if (!_core.HasComponent<CameraComponent>(entity))
                continue;

            auto &cam = _core.GetComponent<CameraComponent>(entity);
            if (!cam.mainCamera)
                continue;

            cam.target = playerPos.position;
        }
    }
};
