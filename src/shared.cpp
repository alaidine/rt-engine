#include <stdlib.h>
#include <limits.h>

// nbnet implementation
#define NBNET_IMPL 

#include "shared.h"

// Command line options
enum
{
    OPT_MESSAGES_COUNT,
    OPT_PACKET_LOSS,
    OPT_PACKET_DUPLICATION,
    OPT_PING,
    OPT_JITTER
};

static Options options = { 0 };

ChangeColorMessage* ChangeColorMessage_Create(void)
{
    return (ChangeColorMessage*)malloc(sizeof(ChangeColorMessage));
}

void ChangeColorMessage_Destroy(ChangeColorMessage* msg)
{
    free(msg);
}

int ChangeColorMessage_Serialize(ChangeColorMessage* msg, NBN_Stream* stream)
{
    NBN_SerializeUInt(stream, msg->color, 0, MAX_COLORS - 1);

    return 0;
}

UpdateStateMessage* UpdateStateMessage_Create(void)
{
    return (UpdateStateMessage*)malloc(sizeof(UpdateStateMessage));
}

void UpdateStateMessage_Destroy(UpdateStateMessage* msg)
{
    free(msg);
}

int UpdateStateMessage_Serialize(UpdateStateMessage* msg, NBN_Stream* stream)
{
    NBN_SerializeUInt(stream, msg->x, 0, GAME_WIDTH);
    NBN_SerializeUInt(stream, msg->y, 0, GAME_HEIGHT);
    NBN_SerializeFloat(stream, msg->val, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);

    return 0;
}

GameStateMessage* GameStateMessage_Create(void)
{
    return (GameStateMessage*)malloc(sizeof(GameStateMessage));
}

void GameStateMessage_Destroy(GameStateMessage* msg)
{
    free(msg);
}

int GameStateMessage_Serialize(GameStateMessage* msg, NBN_Stream* stream)
{
    NBN_SerializeUInt(stream, msg->client_count, 0, MAX_CLIENTS);

    for (unsigned int i = 0; i < msg->client_count; i++)
    {
        NBN_SerializeUInt(stream, msg->client_states[i].client_id, 0, UINT_MAX);
        NBN_SerializeUInt(stream, msg->client_states[i].color, 0, MAX_COLORS - 1);
        NBN_SerializeUInt(stream, msg->client_states[i].x, 0, GAME_WIDTH);
        NBN_SerializeUInt(stream, msg->client_states[i].y, 0, GAME_HEIGHT);
        NBN_SerializeFloat(stream, msg->client_states[i].val, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
    }

    return 0;
}

// Parse the command line
int ReadCommandLine(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--packet_loss") == 0)
        {
            if (i + 1 >= argc)
                return -1; // Missing argument
            options.packet_loss = atof(argv[i + 1]);
            i++; // Skip the argument value
        }
        else if (strcmp(argv[i], "--packet_duplication") == 0)
        {
            if (i + 1 >= argc)
                return -1; // Missing argument
            options.packet_duplication = atof(argv[i + 1]);
            i++; // Skip the argument value
        }
        else if (strcmp(argv[i], "--ping") == 0)
        {
            if (i + 1 >= argc)
                return -1; // Missing argument
            options.ping = atof(argv[i + 1]);
            i++; // Skip the argument value
        }
        else if (strcmp(argv[i], "--jitter") == 0)
        {
            if (i + 1 >= argc)
                return -1; // Missing argument
            options.jitter = atof(argv[i + 1]);
            i++; // Skip the argument value
        }
        else
        {
            // Unknown option
            return -1;
        }
    }

    return 0;
}

// Return the command line options
Options GetOptions(void)
{
    return options;
}
