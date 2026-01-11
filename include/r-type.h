#pragma once

#include "framework.h"
#include "Networking.h"
#include "raylib.h"

#define PROTOCOL_NAME "rt-protocol"
#define PORT 42042

#define TICK_RATE 60 // Simulation tick rate

// Window size, used to display window but also to cap the serialized position values within messages
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define MIN_FLOAT_VAL -1000 // Minimum value of networked client float value
#define MAX_FLOAT_VAL 1000  // Maximum value of networked client float value

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

// Client timeout in ticks (if no message received for this many ticks, client is considered disconnected)
#define CLIENT_TIMEOUT_TICKS 180

// Message types
enum MessageType : uint8_t {
    MSG_CONNECT_REQUEST = 1,
    MSG_CONNECT_ACCEPT,
    MSG_CONNECT_REJECT,
    MSG_DISCONNECT,
    MSG_UPDATE_STATE,
    MSG_GAME_STATE,
    MSG_HEARTBEAT
};

typedef struct {
    Vector2 pos;
    Rectangle rect;
    uint32_t currentFrame;
    uint32_t framesSpeed;
    uint32_t framesCounter;
} Missile;

// Mob state, represents an enemy mob over the network
typedef struct {
    uint32_t mob_id;
    float x;
    float y;
    bool active;
} MobState;

// Messages

typedef struct {
    int x;
    int y;
    unsigned int missile_count;
    Missile missiles[MAX_MISSILES_CLIENT];
} UpdateStateMessage;

// Client state, represents a client over the network
typedef struct {
    uint32_t client_id;
    int x;
    int y;
    unsigned int missile_count;
    Missile missiles[MAX_MISSILES_CLIENT];
} ClientState;

typedef struct {
    unsigned int client_count;
    ClientState client_states[MAX_CLIENTS];
    unsigned int mob_count;
    MobState mobs[MAX_MOBS];
    // Wave system info
    float countdown_timer;     // Timer avant prochaine vague (en secondes)
    unsigned int current_wave; // Num�ro de la vague actuelle (0 = attente initiale)
    bool wave_active;          // Si une vague est en cours
} GameStateMessage;

// Connection accept data sent to client
typedef struct {
    uint32_t client_id;
    int spawn_x;
    int spawn_y;
} ConnectAcceptData;

// Serialization functions using NetBuffer
void SerializeMissile(Roar::NetBuffer &buffer, const Missile &missile);
Missile DeserializeMissile(Roar::NetBuffer &buffer);

void SerializeUpdateStateMessage(Roar::NetBuffer &buffer, const UpdateStateMessage &msg);
UpdateStateMessage DeserializeUpdateStateMessage(Roar::NetBuffer &buffer);

void SerializeClientState(Roar::NetBuffer &buffer, const ClientState &state);
ClientState DeserializeClientState(Roar::NetBuffer &buffer);

void SerializeMobState(Roar::NetBuffer &buffer, const MobState &mob);
MobState DeserializeMobState(Roar::NetBuffer &buffer);

void SerializeGameStateMessage(Roar::NetBuffer &buffer, const GameStateMessage &msg);
GameStateMessage DeserializeGameStateMessage(Roar::NetBuffer &buffer);

void SerializeConnectAcceptData(Roar::NetBuffer &buffer, const ConnectAcceptData &data);
ConnectAcceptData DeserializeConnectAcceptData(Roar::NetBuffer &buffer);
