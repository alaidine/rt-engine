#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

#include <filesystem>

struct Transform2D {
    glm::vec2 pos;
    glm::vec2 size;
};

struct RectangleShape {
    unsigned char color[4];
};

struct ScriptComponent {
    std::string name;
};
