#include "Prefab.h"

namespace Prefab {

Entity MakeEnemy(Scene &_core, float posX, float posY) {
    Entity e = _core.CreateEntity();
    EnemySprite sprite;

    sprite.texture = LoadTexture("ressources/sprites/mob_bydo_minions.png");
    _core.AddComponent(e, Position{Vector2{posX, posY}});
    _core.AddComponent(e, Sprite{GREEN});
    _core.AddComponent(e, Collider{
                              Rectangle{posX, posY, 40, 40},
                              false,
                          });
    _core.AddComponent(
        e, AnimationComponent{
               Rectangle{0, 0, 0, 0},
               {Rectangle{0, 0, 0, 0}, Rectangle{0, 0, 0, 0}, Rectangle{0, 0, 0, 0}, Rectangle{0, 0, 0, 0}, Rectangle{0, 0, 0, 0}

               },
               0,
               0,
               8,
           });
    _core.AddComponent(e, Tag{false});
    _core.AddComponent(e, sprite);
    return e;
}

Entity MakeMilssile(Scene &_core) {
    Entity e = _core.CreateEntity();
    _core.AddComponent(e, AnimationComponent{
                              Rectangle{0, 0, 0, 0},
                              {Rectangle{0, 128, 25, 22}, Rectangle{25, 128, 31, 22}, Rectangle{56, 128, 40, 22},
                               Rectangle{96, 128, 55, 22}, Rectangle{151, 128, 72, 22}},
                              0,
                              0,
                              8,
                          });
    _core.AddComponent(e, Sprite{RED});
    _core.AddComponent(e, Position{Vector2{0, 0}});
    _core.AddComponent(e, Tag{false});
    _core.AddComponent(e, MissileTag{});
    return e;
}

Entity MakePlayer(Scene &_core, float x, float y) {
    Entity e = _core.CreateEntity();
    PlayerSprite sprite;
    sprite.texture = LoadTexture("resources/sprites/player_r-9c_war-head.png");
    _core.AddComponent(e, Position{Vector2{x, y}});
    _core.AddComponent(e, InputController{});
    _core.AddComponent(e, sprite);
    _core.AddComponent(e, AnimationComponent{
                              Rectangle{0, 30, 32, 22},
                          });
    _core.AddComponent(e, Tag{true});
    _core.AddComponent(e, Sprite{WHITE});
    _core.AddComponent(e, playerCooldown{false});
    _core.AddComponent(e, Collider{
                              Rectangle{0, 30, 32, 22},
                              true,
                          });
    _core.AddComponent(e, Velocity{0, 0});
    _core.AddComponent(e, LocalPlayerTag{});
    // _core.AddComponent(e, CameraComponent{
    //     {0.0f, 0.0f},
    //     {0.0f, 0.0f},
    //     {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f},
    //     1.0f,
    //     0.0f,
    //     true,
    // });
    return e;
}

Entity MakeCamera(Scene &_core) {
    Entity e = _core.CreateEntity();
    _core.AddComponent(e, CameraComponent{
                              {0.0f, 0.0f},
                              {0.0f, 0.0f},
                              10.0f,
                              0.0f,
                              true,
                          });
    return e;
}
} // namespace Prefab
