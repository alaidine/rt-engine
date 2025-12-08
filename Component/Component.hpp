#pragma once
    #include <cstdint>
    #include <raylib.h>

using ComponentType = std::uint8_t;

const ComponentType MAX_COMPONENTS = 32;

struct Gravity {
    float force;
};

struct Position {
    Vector2 position;
};

struct Velocity {
    float speed;
};

struct Sprite {
    Color color;
};

struct InputController{};