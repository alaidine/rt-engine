 #pragma once
    #include "../Core.hpp"
    #include <raylib.h>

extern Core _core;

class RendererSystem : public System {
    public:
        void Update() override {
            for (auto const& entity: _entities) {
                auto& pos = _core.GetComponent<Position>(entity);
                auto& sprite = _core.GetComponent<Sprite>(entity);
                std::cout << "Renderer system" << std::endl;
                DrawCircleV(pos.position, 50, sprite.color);
            }
        }
};