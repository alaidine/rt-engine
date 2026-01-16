#pragma once
    #include "Scene.h"
    #include "CollisionSystem.h"
    #include "VelocitySystem.h"

class PyhsicEngine {
    private:
        CollisionSystem& _collisionSystem;
        VelocitySystem& _velocitySystem;
    public:
        PyhsicEngine(CollisionSystem& collisionSystem, VelocitySystem& velocitySystem)
            : _collisionSystem(collisionSystem), _velocitySystem(velocitySystem) {}
        void updateAll() {
            _velocitySystem.Update();
            _collisionSystem.Update();
        }
};
