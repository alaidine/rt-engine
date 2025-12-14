# ECS Overview

This project uses an **Entity-Component-System (ECS)** architecture to structure the game. The ECS separates data (components) from logic (systems), allowing for flexible and efficient management of entities in the game world.

## Key Concepts

- **Entity**: A unique identifier (usually an integer) that represents a game object. Entities themselves contain no logic or data.
- **Component**: Pure data attached to an entity. Examples include `Position`, `Velocity`, `AnimationComponent`, and `Sprite`.
- **System**: Logic that operates on entities possessing certain components. For example, a `RendererSystem` updates all entities with `Position` and `Sprite` components.
- **Core**: The ```central manager``` of the ECS that handles ```entity creation```, ```component storage```, and ```system registration```, ensuring systems can access the correct entities and components.
---

## How the ECS Works

1. **Create entities**:

```cpp
Entity e = _core.CreateEntity();
```

2. ***Add Component**:

```cpp
_core.AddComponent(e, Position{Vector2{x, y}});
_core.AddComponent(e, Sprite{WHITE});
```
3. ***Create systems*** by inheriting from the ```System``` base class and overriding ```Update()```:

```cpp
_core.AddComponent(e,Position{Vector2{x, y}});
_core.AddComponent(e, Sprite{WHITE});
```

4. ***System Processing:*** Each system automatically iterates over all ```entities``` that have the required components.

---
**ADDING NEW COMPONENT**

1. ***Deine the component***
```cpp
struct Health {
    int current;
    int max;
};
```

2. ***Register the compoent by adding it here:***
```cpp
    registerTest.RegisterComponents<Position, Gravity, Velocity, Sprite, InputController, PlayerSprite, AnimationComponent, Tag, MissileTag, playerCooldown, Health>(_core);
```

3. ***Add it to an entity***
```cpp
_core.AddComponent(playerEntity, Health{100, 100});
```


4. ***Access it in a system***
```cpp
auto& health = _core.GetComponent<Health>(entity);
```
---

**ADDING A NEW SYSTEM**

1. ***Create a class inheriting from ```System```:***
```cpp
class HealthSystem : public System {
    void Update() override {
        for (auto& entity : _entities) {
            auto& health = _core.GetComponent<Health>(entity);
            if (health.current <= 0) {
                // Handle entity death
            }
        }
    }
};
```

2. ***Register the system with the ```ECS```:***
```cpp
auto healthSystem = _core.RegisterSystem<healthSystem>();
heatlSystem->order = 1;
```
_```order```_ defines the update priority among systems

2. ***The system will automatically operate on all entities with the required components.***

---

**PREFABS**

Prefabs are helper ```functions``` to quickly create ```entities``` with predefined ```components```:
```cpp
Entity MakePlayer(Core& _core, float x, float y) {
    Entity e = _core.CreateEntity();
    _core.AddComponent(e, Position{Vector2{x, y}});
    _core.AddComponent(e, Sprite{WHITE});
    _core.AddComponent(e, PlayerTag{});
    return e;
}
```
---
**NOTES**

* Always add components before systems attempt to access them.

* Systems should only operate on entities with the required components to avoid undefined behavior.

* The ECS is designed to separate concerns: ```components store data```, ```systems implement logic```, ```entities combine both```.
