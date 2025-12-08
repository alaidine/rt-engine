#pragma once
    #include "../Core.hpp"
    #include "System.hpp"
    #include "../Component/Component.hpp"

extern Core _core;

class InputControllerSystem : public System {
    public:
        void Update() override {
            for (auto const& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& velocity = _core.GetComponent<Velocity>(entity);
                auto& input = _core.GetComponent<InputController>(entity);

                std::cout << "Input controller system" << std::endl;

                if ((IsKeyDown(KEY_RIGHT)))
                    pos.position.x += velocity.speed;
                if ((IsKeyDown(KEY_LEFT)))
                    pos.position.x -= velocity.speed;
                if ((IsKeyDown(KEY_UP)))
                    pos.position.y -= velocity.speed;
                if ((IsKeyDown(KEY_DOWN)))
                    pos.position.y += velocity.speed;
            }
        }
};