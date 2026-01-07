#include "GuiSystem.h"
#include "raygui.h"

namespace Roar {
    
ButtonSystem::ButtonSystem() {}

ButtonSystem::~ButtonSystem() {}

void ButtonSystem::Init() {}

void ButtonSystem::Update(Scene &scene) {
    for (auto const &entity : mEntities) {
        auto &button = scene.GetComponent<ButtonComponent>(entity);
        auto &transform = scene.GetComponent<TransformComponent>(entity);

        button.clicked = GuiButton(Rectangle{transform.pos.x, transform.pos.y, transform.size.x, transform.size.y},
            button.text.c_str());
    }
}

InputSystem::InputSystem() {}

InputSystem::~InputSystem() {}

void InputSystem::Init() {}

void InputSystem::Update(Scene &scene) {}

} // namespace Roar
