#pragma once

#include "raylib.h"
#include "framework.h"

#define NBN_LogInfo(...) TraceLog(LOG_INFO, __VA_ARGS__)
#define NBN_LogError(...) TraceLog(LOG_ERROR, __VA_ARGS__)
#define NBN_LogWarning(...) TraceLog(LOG_WARNING, __VA_ARGS__)
#define NBN_LogDebug(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
#define NBN_LogTrace(...) TraceLog(LOG_TRACE, __VA_ARGS__)

extern "C" {
#ifndef NBNET_IMPL
#include "nbnet.h"
#include "udp.h"
#endif
}

#define PROTOCOL_NAME "rt-protocol"
#define PORT 42042

#define TICK_RATE 60 // Simulation tick rate

// Window size, used to display window but also to cap the serialized position values within messages
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define MIN_FLOAT_VAL -5 // Minimum value of networked client float value
#define MAX_FLOAT_VAL 5 // Maximum value of networked client float value

// Maximum number of connected clients at a time
#define MAX_CLIENTS 4

// Max number of colors for client to switch between
#define MAX_COLORS 7

// A code passed by the server when closing a client connection due to being full (max client count reached)
#define SERVER_FULL_CODE 42

// Message ids
enum
{
    CHANGE_COLOR_MESSAGE,
    UPDATE_STATE_MESSAGE,
    GAME_STATE_MESSAGE
};

// Messages

typedef struct
{
    int x;
    int y;
    float val;
} UpdateStateMessage;

// Client colors used for ChangeColorMessage and GameStateMessage messages
typedef enum
{
    CLI_RED,
    CLI_GREEN,
    CLI_BLUE,
    CLI_YELLOW,
    CLI_ORANGE,
    CLI_PURPLE,
    CLI_PINK
} ClientColor;

typedef struct
{
    ClientColor color;
} ChangeColorMessage;

// Client state, represents a client over the network
typedef struct
{
    uint32_t client_id;
    int x;
    int y;
    float val;
    ClientColor color;
} ClientState;

typedef struct
{
    unsigned int client_count;
    ClientState client_states[MAX_CLIENTS];
} GameStateMessage;

// Store all options from the command line
typedef struct
{
    float packet_loss;
    float packet_duplication;
    float ping;
    float jitter;
} Options;


ChangeColorMessage* ChangeColorMessage_Create(void);
void ChangeColorMessage_Destroy(ChangeColorMessage*);
int ChangeColorMessage_Serialize(ChangeColorMessage* msg, NBN_Stream*);

UpdateStateMessage* UpdateStateMessage_Create(void);
void UpdateStateMessage_Destroy(UpdateStateMessage*);
int UpdateStateMessage_Serialize(UpdateStateMessage*, NBN_Stream*);

GameStateMessage* GameStateMessage_Create(void);
void GameStateMessage_Destroy(GameStateMessage*);
int GameStateMessage_Serialize(GameStateMessage*, NBN_Stream*);

int ReadCommandLine(int, char* []);
Options GetOptions(void);
