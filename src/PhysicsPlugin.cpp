#include "IPhysics.h"

#include "box2d/box2d.h"
#include "raylib.h"

#include <cassert>

constexpr uint32_t GROUND_COUNT = 14;
constexpr uint32_t BOX_COUNT = 10;

namespace Roar {
namespace Physics {

typedef struct Entity {
    b2BodyId bodyId;
    b2Vec2 extent;
    Texture texture;
} Entity;

} // namespace Physics
} // namespace Roar

uint32_t WIDTH = 1920;
uint32_t HEIGHT = 1080;

bool pause = false;
Texture groundTexture;
Texture boxTexture;
float lengthUnitsPerMeter;
b2WorldDef worldDef;
b2WorldId worldId;
b2Vec2 groundExtent;
b2Vec2 boxExtent;
b2Polygon groundPolygon;
b2Polygon boxPolygon;

Roar::Physics::Entity groundEntities[GROUND_COUNT] = {0};
Roar::Physics::Entity boxEntities[BOX_COUNT] = {0};

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

struct ShapeUserData
{
	float maxPush;
	bool clipVelocity;
};

enum CollisionBits : uint64_t
{
	StaticBit = 0x0001,
	MoverBit = 0x0002,
	DynamicBit = 0x0004,
	DebrisBit = 0x0008,

	AllBits = ~0u,
};

void CreateCapsule() {
    b2Capsule m_capsule;
	b2BodyId m_elevatorId;
	b2ShapeId m_ballId;
	ShapeUserData m_friendlyShape;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = {32.0f, 4.5f};

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    m_friendlyShape.maxPush = 0.025f;
    m_friendlyShape.clipVelocity = false;

    shapeDef.filter = {MoverBit, AllBits, 0};
    shapeDef.userData = &m_friendlyShape;
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2CreateCapsuleShape(bodyId, &shapeDef, &m_capsule);
}

void Box2DPhysics::InitDemo(uint32_t width, uint32_t height) {
    WIDTH = width;
    HEIGHT = height;

    // 128 pixels per meter is a appropriate for this scene. The boxes are 128 pixels wide.
    float lengthUnitsPerMeter = 128.0f;
    b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);

    worldDef = b2DefaultWorldDef();

    // Realistic gravity is achieved by multiplying gravity by the length unit.
    worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
    worldId = b2CreateWorld(&worldDef);

    groundTexture = LoadTexture("resources/sprites/ground.png");
    boxTexture = LoadTexture("resources/sprites/box.png");

    groundExtent = {0.5f * groundTexture.width, 0.5f * groundTexture.height};
    boxExtent = {0.5f * boxTexture.width, 0.5f * boxTexture.height};

    // These polygons are centered on the origin and when they are added to a body they
    // will be centered on the body position.
    groundPolygon = b2MakeBox(groundExtent.x, groundExtent.y);
    boxPolygon = b2MakeBox(boxExtent.x, boxExtent.y);

    for (int i = 0; i < GROUND_COUNT; ++i) {
        Entity *entity = groundEntities + i;
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = b2Vec2{(2.0f * i + 2.0f) * groundExtent.x, HEIGHT - groundExtent.y - 100.0f};

        // I used this rotation to test the world to screen transformation
        // bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

        entity->bodyId = b2CreateBody(worldId, &bodyDef);
        entity->extent = groundExtent;
        entity->texture = groundTexture;
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(entity->bodyId, &shapeDef, &groundPolygon);
    }

    int boxIndex = 0;
    for (int i = 0; i < 4; ++i) {
        float y = HEIGHT - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * boxExtent.y - 20.0f;

        for (int j = i; j < 4; ++j) {
            float x = 0.5f * HEIGHT + (3.0f * j - i - 3.0f) * boxExtent.x;
            assert(boxIndex < BOX_COUNT);

            Entity *entity = boxEntities + boxIndex;
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = b2Vec2{x, y};
            entity->bodyId = b2CreateBody(worldId, &bodyDef);
            entity->texture = boxTexture;
            entity->extent = boxExtent;
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            b2CreatePolygonShape(entity->bodyId, &shapeDef, &boxPolygon);

            boxIndex += 1;
        }
    }

    pause = false;
}

void Box2DPhysics::UpdateDemo(void) {
    if (IsKeyPressed(KEY_P)) {
        pause = !pause;
    }

    if (pause == false) {
        float deltaTime = GetFrameTime();
        b2World_Step(worldId, deltaTime, 4);
    }
    ClearBackground(DARKGRAY);

    const char *message = "Hello Box2D!";
    int fontSize = 36;
    int textWidth = MeasureText("Hello Box2D!", fontSize);
    DrawText(message, (WIDTH - textWidth) / 2, 50, fontSize, LIGHTGRAY);

    for (int i = 0; i < GROUND_COUNT; ++i) {
        DrawEntity(groundEntities + i);
    }

    for (int i = 0; i < BOX_COUNT; ++i) {
        DrawEntity(boxEntities + i);
    }
}

void Box2DPhysics::CleanupDemo(void) {
    UnloadTexture(groundTexture);
    UnloadTexture(boxTexture);
}

} // namespace Physics
} // namespace Roar

extern "C" PLUGIN_EXPORT Roar::IPlugin *CreatePlugin() { return new Roar::Physics::Box2DPhysics(); }
