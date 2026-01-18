#pragma once

#include "IPhysics.h"
#include "box2d/box2d.h"
#include "raylib.h"

#include <cassert>
#include <cmath>

constexpr uint32_t GROUND_COUNT = 14;
constexpr uint32_t BOX_COUNT = 10;

namespace Roar {
namespace Physics {

typedef struct Entity {
    b2BodyId bodyId;
    b2Vec2 extent;
    Color color;
} Entity;

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
    uint32_t WIDTH = 1920;
    uint32_t HEIGHT = 1080;

    struct DemoData {
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
    } demoData;

    b2WorldDef worldDef;
    b2WorldId worldId;
    float lengthUnitsPerMeter;
};

} // namespace Physics
} // namespace Roar
