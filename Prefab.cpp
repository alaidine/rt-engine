#include "Prefab.hpp"

namespace Prefab {
    Entity MakePlayer(Core& _core, float posX, float posY) {
        Entity e = _core.CreateEntity();
        _core.AddComponent(e, Position{Vector2{posX, posY}});
        // _core.AddComponent(e, Gravity{9.81});
        _core.AddComponent(e, Sprite{RED});
        _core.AddComponent(e, Velocity{4});
        _core.AddComponent(e, InputController{});
        
        return e;
    }

    Entity MakeEnemy(Core& _core, float posX, float posY) {
        Entity e = _core.CreateEntity();
        _core.AddComponent(e, Position{Vector2{posX, posY}});
        _core.AddComponent(e, Gravity{9.81});
        _core.AddComponent(e, Sprite{GREEN});
        _core.AddComponent(e, Velocity{4});
        
        return e;
    }
}