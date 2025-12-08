#pragma once
    #include "Core.hpp"

namespace Prefab {
    Entity MakePlayer(Core& _core, float posX, float posY);
    Entity MakeEnemy(Core& _core, float posX, float posY);
}