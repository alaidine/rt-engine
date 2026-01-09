#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <filesystem>
#include <glm/glm.hpp>
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

struct TransformComponent {
    glm::vec2 pos;
    glm::vec2 size;
};

struct RectangleComponent {
    unsigned char color[4];
};

struct ScriptComponent {
    std::string name;
};

} // namespace Roar
