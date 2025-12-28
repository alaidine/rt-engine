#pragma once

#include "Component.h"
#include "ECS.h"
#include "Scripting.h"

class ScriptSystem : public System {
  public:
    ScriptSystem();
    ~ScriptSystem();

    void Init();
    void Update(float dt);
};
