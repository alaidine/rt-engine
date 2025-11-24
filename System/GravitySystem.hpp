#pragma once
    #include "../Core.hpp"
    #include "System.hpp"
    #include "../Component/Component.hpp"

extern Core _core;

class GravitySystem : public System {
    public:
        void Update() {
            for (auto const& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& gravity = _core.GetComponent<Gravity>(entity);
                pos.y += gravity.force;
                std::cout << "pos y = " << pos.y << std::endl; 
            }
        }
};