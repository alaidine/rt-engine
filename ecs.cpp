#include "Core.hpp"
#include "System/GravitySystem.hpp"

Core _core;

int main() {
    _core.Init();
    
    _core.RegisterComponent<Position>();
    _core.RegisterComponent<Gravity>();
    _core.RegisterComponent<Velocity>();

    auto gravitySystem = _core.RegisterSystem<GravitySystem>();

    Signature gravitySignature;

    gravitySignature.set(_core.GetComponentType<Gravity>(), true);
    gravitySignature.set(_core.GetComponentType<Position>(), true);

    _core.SetSystemSignature<GravitySystem>(gravitySignature);

    Entity player = _core.CreateEntity();
    _core.AddComponent(player, Position{4.0f, 2.0f});
    _core.AddComponent(player, Gravity{-9.81f});

    Entity ship = _core.CreateEntity();
    _core.AddComponent(ship, Position{4.0f, 2.0f});

    gravitySystem->Update();
    return 0;
}