#pragma once

#include "Component.h"
#include "ECS.h"
#include "Scripting.h"

namespace Roar {

class ScriptSystem : public System {
  public:
    ScriptSystem();
    ~ScriptSystem();

    void Init();
    void Update(float dt);
    void Reload();
};

} // namespace Roar
