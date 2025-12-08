#pragma once
    #include "../Core.hpp"
    #include "System.hpp"
    #include "../Component/Component.hpp"

extern Core _core;

class GravitySystem : public System {
    public:
        void Update() override {
            for (auto const& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& gravity = _core.GetComponent<Gravity>(entity);
                pos.position.y += gravity.force;
                if (pos.position.y >= 400)
                    pos.position.y = 400;
                std::cout << "Gravity system" << std::endl;
            }
        }
};