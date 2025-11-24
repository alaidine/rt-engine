#pragma once
    #include <cstdint>

using ComponentType = std::uint8_t;

const ComponentType MAX_COMPONENTS = 32;

struct Gravity {
    float force;
};

struct Position {
    float x;
    float y;
};

struct Velocity {
    float speed;
};
