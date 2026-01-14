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

    int boxIndex = 0;
    for (int i = 0; i < 4; ++i) {
        float y = HEIGHT - demoData.groundExtent.y - 100.0f - (2.5f * i + 2.0f) * demoData.boxExtent.y - 20.0f;

        for (int j = i; j < 4; ++j) {
            float x = 0.5f * HEIGHT + (3.0f * j - i - 3.0f) * demoData.boxExtent.x;
            assert(boxIndex < BOX_COUNT);

            Entity *entity = demoData.boxEntities + boxIndex;
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = b2Vec2{x, y};
            entity->bodyId = b2CreateBody(demoData.worldId, &bodyDef);
            entity->texture = demoData.boxTexture;
            entity->extent = demoData.boxExtent;
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            b2CreatePolygonShape(entity->bodyId, &shapeDef, &demoData.boxPolygon);

            boxIndex += 1;
        }
    }

    b2BodyDef capsuleBodyDef = b2DefaultBodyDef();
    capsuleBodyDef.position = b2Vec2{float(HEIGHT / 2), float(WIDTH / 2)};

    demoData.pause = false;
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

    for (int i = 0; i < BOX_COUNT; ++i) {
        DrawEntity(demoData.boxEntities + i);
    }
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
