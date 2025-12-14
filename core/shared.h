#pragma once

#include "raylib.h"
#include "framework.h"

#define NBN_LogInfo(...) TraceLog(LOG_INFO, __VA_ARGS__)
#define NBN_LogError(...) TraceLog(LOG_ERROR, __VA_ARGS__)
#define NBN_LogWarning(...) TraceLog(LOG_WARNING, __VA_ARGS__)
#define NBN_LogDebug(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#define NBN_LogTrace(...) TraceLog(LOG_TRACE, __VA_ARGS__)

#include "nbnet.h"
#include "udp.h"

#define PROTOCOL_NAME "rt-protocol"
#define PORT 42042

#define TICK_RATE 60 // Simulation tick rate

// Window size, used to display window but also to cap the serialized position values within messages
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define MIN_FLOAT_VAL -1000 // Minimum value of networked client float value
#define MAX_FLOAT_VAL 1000 // Maximum value of networked client float value

// Maximum number of connected clients at a time
#define MAX_CLIENTS 4

// Max number of colors for client to switch between
#define MAX_COLORS 7

// Max number of missiles that can be handled by the server
#define MAX_MISSILES 400

// Max number of missiles that can be sent to the server
#define MAX_MISSILES_CLIENT 100

// Max number of mobs in the game
#define MAX_MOBS 20

// Mob spawn interval in ticks
#define MOB_SPAWN_INTERVAL 120

// Mob movement speed
#define MOB_SPEED 2

// Mob size
#define MOB_WIDTH 40
#define MOB_HEIGHT 40

// A code passed by the server when closing a client connection due to being full (max client count reached)
#define SERVER_FULL_CODE 42

// Message ids
enum
{
    UPDATE_STATE_MESSAGE,
    MISSILES_STATE_MESSAGE,
    GAME_STATE_MESSAGE
};

typedef struct
{
    Vector2 pos;
    Rectangle rect;
    uint32_t currentFrame;
    uint32_t framesSpeed;
    uint32_t framesCounter;
} Missile;

// Mob state, represents an enemy mob over the network
typedef struct
{
    uint32_t mob_id;
    float x;
    float y;
    bool active;
} MobState;

// Messages

typedef struct
{
    int x;
    int y;
    unsigned int missile_count;
    Missile missiles[MAX_MISSILES_CLIENT];
} UpdateStateMessage;

// Client state, represents a client over the network
typedef struct
{
    uint32_t client_id;
    int x;
    int y;
    unsigned int missile_count;
    Missile missiles[MAX_MISSILES_CLIENT];
} ClientState;

typedef struct
{
    unsigned int client_count;
    ClientState client_states[MAX_CLIENTS];
    unsigned int mob_count;
    MobState mobs[MAX_MOBS];
} GameStateMessage;

// Store all options from the command line
typedef struct
{
    float packet_loss;
    float packet_duplication;
    float ping;
    float jitter;
} Options;

UpdateStateMessage* UpdateStateMessage_Create(void);
void UpdateStateMessage_Destroy(UpdateStateMessage*);
int UpdateStateMessage_Serialize(UpdateStateMessage*, NBN_Stream*);

GameStateMessage* GameStateMessage_Create(void);
void GameStateMessage_Destroy(GameStateMessage*);
int GameStateMessage_Serialize(GameStateMessage*, NBN_Stream*);

int ReadCommandLine(int, char* []);
Options GetOptions(void);
