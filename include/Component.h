#pragma once
#include <array>
#include <cstdint>
#include <raylib.h>
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define TARGET_FPS 100

using ComponentType = std::uint8_t;

const ComponentType MAX_COMPONENTS = 32;

struct Gravity {
    float force;
};

struct Position {
    Vector2 position;
};

struct Velocity {
    float speedX;
    float speedY;
};

struct Sprite {
    Color color;
};

struct PlayerSprite {
    Texture2D texture;
};

struct EnemySprite {
    Texture2D texture;
};

struct Collider {
    Rectangle rect;
    bool isPlayer;
};

struct AnimationComponent {
    Rectangle rect;
    std::array<Rectangle, 5> _animationRectangle;
    int _current_frame;
    int _frameCounter;
    int _frameSpeed;
};

struct InputController {};

struct Tag {
    bool isPlayer;
};

struct EnemyTag {
    bool isEnemy;
};

struct MissileTag {};

struct playerCooldown {
    bool canFire;
};

struct CameraComponent {
    Vector2 position;
    Vector2 target;
    Vector2 offset;
    float zoom;
    float rotation;
    bool mainCamera;
};

// Networked client component for multiplayer
struct NetworkedClient {
    uint32_t client_id;
    bool is_local;
};

// Tag to identify local player entity
struct LocalPlayerTag {};

// Tag to identify remote player entities
struct RemotePlayerTag {};
