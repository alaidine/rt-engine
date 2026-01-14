#include "Box2DPhysics.h"

#include "box2d/box2d.h"
#include "raylib.h"

#include <cassert>
#include <cmath>

namespace Roar {
namespace Physics {

void DrawEntity(const Entity *entity) {
    // The boxes were created centered on the bodies, but raylib draws textures starting at the top left corner.
    // b2Body_GetWorldPoint gets the top left corner of the box accounting for rotation.
    b2Vec2 p = b2Body_GetWorldPoint(entity->bodyId, b2Vec2{-entity->extent.x, -entity->extent.y});
    b2Rot rotation = b2Body_GetRotation(entity->bodyId);
    float radians = b2Rot_GetAngle(rotation);

    Vector2 ps = {p.x, p.y};
    DrawTextureEx(entity->texture, ps, RAD2DEG * radians, 1.0f, WHITE);

    // I used these circles to ensure the coordinates are correct
    // DrawCircleV(ps, 5.0f, BLACK);
    // p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){0.0f, 0.0f});
    // ps = (Vector2){ p.x, p.y };
    // DrawCircleV(ps, 5.0f, BLUE);
    // p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){ entity->extent.x, entity->extent.y });
    // ps = (Vector2){ p.x, p.y };
    // DrawCircleV(ps, 5.0f, RED);
}

void CreateCapsule(b2WorldId worldId, b2Vec2 position) {}

void DrawCapsule(const b2Capsule *capsule, b2BodyId bodyId, Color color) {
    b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, capsule->center1);
    b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, capsule->center2);

    // Draw the two circles at the ends of the capsule
    DrawCircleV({p1.x, p1.y}, capsule->radius, color);
    DrawCircleV({p2.x, p2.y}, capsule->radius, color);

    // Calculate the rectangle for the body of the capsule
    // Vector from p1 to p2
    b2Vec2 p1p2 = b2Vec2{p2.x - p1.x, p2.y - p1.y};
    float length = b2Length(p1p2);

    // Normalize the vector
    b2Vec2 normal = b2Vec2{p1p2.x / length, p1p2.y / length};

    // Perpendicular vector for the width of the rectangle
    b2Vec2 perp = b2Vec2{-normal.y, normal.x};

    // Calculate the four corners of the rectangle
    Vector2 v1 = {p1.x + perp.x * capsule->radius, p1.y + perp.y * capsule->radius};
    Vector2 v2 = {p1.x - perp.x * capsule->radius, p1.y - perp.y * capsule->radius};
    Vector2 v3 = {p2.x - perp.x * capsule->radius, p2.y - perp.y * capsule->radius};
    Vector2 v4 = {p2.x + perp.x * capsule->radius, p2.y + perp.y * capsule->radius};

    DrawTriangle(v1, v2, v3, color);
    DrawTriangle(v1, v3, v4, color);
}

void Box2DPhysics::InitDemo(uint32_t width, uint32_t height) {
    WIDTH = width;
    HEIGHT = height;

    // 128 pixels per meter is a appropriate for this scene. The boxes are 128 pixels wide.
    float lengthUnitsPerMeter = 128.0f;
    b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);

    demoData.worldDef = b2DefaultWorldDef();

    // Realistic gravity is achieved by multiplying gravity by the length unit.
    demoData.worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
    demoData.worldId = b2CreateWorld(&demoData.worldDef);

    demoData.groundTexture = LoadTexture("resources/sprites/ground.png");
    demoData.boxTexture = LoadTexture("resources/sprites/box.png");

    demoData.groundExtent = {0.5f * demoData.groundTexture.width, 0.5f * demoData.groundTexture.height};
    demoData.boxExtent = {0.5f * demoData.boxTexture.width, 0.5f * demoData.boxTexture.height};

    // These polygons are centered on the origin and when they are added to a body they
    // will be centered on the body position.
    demoData.groundPolygon = b2MakeBox(demoData.groundExtent.x, demoData.groundExtent.y);
    demoData.boxPolygon = b2MakeBox(demoData.boxExtent.x, demoData.boxExtent.y);

    for (int i = 0; i < GROUND_COUNT; ++i) {
        Entity *entity = demoData.groundEntities + i;
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = b2Vec2{(2.0f * i + 2.0f) * demoData.groundExtent.x, HEIGHT - demoData.groundExtent.y - 100.0f};

        // This rotation can be used to test the world to screen transformation
        // bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

        entity->bodyId = b2CreateBody(demoData.worldId, &bodyDef);
        entity->extent = demoData.groundExtent;
        entity->texture = demoData.groundTexture;
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(entity->bodyId, &shapeDef, &demoData.groundPolygon);
    }

    // int boxIndex = 0;
    // for (int i = 0; i < 4; ++i) {
    //     float y = HEIGHT - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * boxExtent.y - 20.0f;

    //     for (int j = i; j < 4; ++j) {
    //         float x = 0.5f * HEIGHT + (3.0f * j - i - 3.0f) * boxExtent.x;
    //         assert(boxIndex < BOX_COUNT);

    //         Entity *entity = boxEntities + boxIndex;
    //         b2BodyDef bodyDef = b2DefaultBodyDef();
    //         bodyDef.type = b2_dynamicBody;
    //         bodyDef.position = b2Vec2{x, y};
    //         entity->bodyId = b2CreateBody(worldId, &bodyDef);
    //         entity->texture = boxTexture;
    //         entity->extent = boxExtent;
    //         b2ShapeDef shapeDef = b2DefaultShapeDef();
    //         b2CreatePolygonShape(entity->bodyId, &shapeDef, &boxPolygon);

    //         boxIndex += 1;
    //     }
    // }

    // Add a capsule
    demoData.capsule = {{0.0f, -0.5f}, {0.0f, 0.5f}, 0.3f};

    b2BodyDef capsuleBodyDef = b2DefaultBodyDef();
    capsuleBodyDef.position = b2Vec2{float(HEIGHT / 2), float(WIDTH / 2)};

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    demoData.friendlyShape.maxPush = 0.025f;
    demoData.friendlyShape.clipVelocity = false;

    shapeDef.filter = {MoverBit, AllBits, 0};
    shapeDef.userData = &demoData.friendlyShape;
    demoData.capsuleId = b2CreateBody(worldId, &capsuleBodyDef);
    b2CreateCapsuleShape(demoData.capsuleId, &shapeDef, &demoData.capsule);

    demoData.pause = false;
}

static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult *planeResult, void *context) {
    assert(planeResult->hit == true);

    Box2DPhysics *self = static_cast<Box2DPhysics *>(context);
    float maxPush = FLT_MAX;
    bool clipVelocity = true;
    ShapeUserData *userData = static_cast<ShapeUserData *>((void *)b2Shape_GetUserData(shapeId));
    if (userData != nullptr) {
        maxPush = userData->maxPush;
        clipVelocity = userData->clipVelocity;
    }

    /*
    if (self->m_planeCount < m_planeCapacity) {
        assert(b2IsValidPlane(planeResult->plane));
        self->m_planes[self->m_planeCount] = {planeResult->plane, maxPush, 0.0f, clipVelocity};
        self->m_planeCount += 1;
    }
    */

    return true;
}

void Box2DPhysics::UpdateDemo(void) {
    if (IsKeyPressed(KEY_P)) {
        demoData.pause = !demoData.pause;
    }

    if (demoData.pause == false) {
        float deltaTime = GetFrameTime();
        b2World_Step(demoData.worldId, deltaTime, 4);
    }
    ClearBackground(DARKGRAY);

    const char *message = "Hello Box2D!";
    int fontSize = 36;
    int textWidth = MeasureText("Hello Box2D!", fontSize);
    DrawText(message, (WIDTH - textWidth) / 2, 50, fontSize, LIGHTGRAY);

    for (int i = 0; i < GROUND_COUNT; ++i) {
        DrawEntity(demoData.groundEntities + i);
    }

    // Mover overlap filter
    b2QueryFilter collideFilter = {MoverBit, StaticBit | DynamicBit | MoverBit};

    b2World_CollideMover(demoData.worldId, &demoData.capsule, collideFilter, PlaneResultFcn, this);
    DrawCapsule(&demoData.capsule, demoData.capsuleId, BLUE);

    // for (int i = 0; i < BOX_COUNT; ++i) {
    //     DrawEntity(boxEntities + i);
    // }
}

void Box2DPhysics::CleanupDemo(void) {
    UnloadTexture(demoData.groundTexture);
    UnloadTexture(demoData.boxTexture);
}

void Box2DPhysics::Startup(void) {
    lengthUnitsPerMeter = 128.0f;
    b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);
    worldDef = b2DefaultWorldDef();
    // Realistic gravity is achieved by multiplying gravity by the length unit.
    worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
    worldId = b2CreateWorld(&demoData.worldDef);
}

void Box2DPhysics::Step(void) {}

void Box2DPhysics::Shutdown(void) {}

} // namespace Physics
} // namespace Roar

extern "C" PLUGIN_EXPORT Roar::IPlugin *CreatePlugin() { return new Roar::Physics::Box2DPhysics(); }
