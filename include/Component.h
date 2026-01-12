#pragma once

#include <filesystem>
#include <string>

namespace Roar {

struct ButtonComponent {
    std::string text;
    bool clicked;
};

struct InputComponent {
    std::string text;
};

struct ClientComponent {
    uint32_t addr;
    uint16_t port;
};

struct ServerComponent {
    uint16_t port;
};

struct TextureComponent {
    std::string path;
    int x;
    int y;
    int width;
    int height;
};

struct Vec2 {
    float x;
    float y;
};

struct TransformComponent {
    Vec2 size;
    Vec2 pos;
};

struct RectangleComponent {
    unsigned char color[4];
};

struct ScriptComponent {
    std::string name;
};

} // namespace Roar
