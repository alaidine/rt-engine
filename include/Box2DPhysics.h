#pragma once

#include "IPhysics.h"
#include "box2d/box2d.h"
#include "raylib.h"

constexpr uint32_t GROUND_COUNT = 5;
constexpr uint32_t BOX_COUNT = 5;

uint32_t WIDTH = 1920;
uint32_t HEIGHT = 1080;

struct ShapeUserData {
    float maxPush;
    bool clipVelocity;
};

enum CollisionBits : uint64_t {
    StaticBit = 0x0001,
    MoverBit = 0x0002,
    DynamicBit = 0x0004,
    DebrisBit = 0x0008,

    AllBits = ~0u,
};

namespace Roar {
namespace Physics {

typedef struct Entity {
    b2BodyId bodyId;
    b2Vec2 extent;
    Texture texture;
};

class Box2DPhysics : public IPhysics {
  public:
    void InitDemo(uint32_t width, uint32_t height) override;
    void UpdateDemo() override;
    void CleanupDemo() override;
    void Startup() override;
    void Step() override;
    void Shutdown() override;
    const char *GetID() const override { return "Box2DPhysics"; }

  private:
    struct DemoData {
        Texture groundTexture;
        Texture boxTexture;
        bool pause;
        float lengthUnitsPerMeter;
        b2WorldDef worldDef;
        b2WorldId worldId;
        b2Vec2 groundExtent;
        b2Vec2 boxExtent;
        b2Polygon groundPolygon;
        b2Polygon boxPolygon;
        Entity groundEntities[GROUND_COUNT];
        Entity boxEntities[BOX_COUNT];
        
        // Character mover stuff
        float throttle;
        b2Capsule capsule;
        b2BodyId capsuleId;
        ShapeUserData friendlyShape;
    } demoData;

    b2WorldDef worldDef;
    b2WorldId worldId;
    float lengthUnitsPerMeter;
};

} // namespace Physics
} // namespace Roar
