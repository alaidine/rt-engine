#pragma once

#include "Component.h"
#include "ECS.h"
#include "Renderer.h"

namespace Roar {

class ButtonSystem : public System {
  public:
    ButtonSystem();
    ~ButtonSystem();

    void Init();
    void Update(Scene &scene);
};

class InputSystem : public System {
  public:
    InputSystem();
    ~InputSystem();

    void Init();
    void Update(Scene &scene);
};

} // namespace Roar
