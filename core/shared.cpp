#include "shared.h"

#include <stdlib.h>
#include <limits.h>

#define NBNET_IMPL
#include "nbnet.h"
#include "udp.h"

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

UpdateStateMessage* UpdateStateMessage_Create(void)
{
    return (UpdateStateMessage*)calloc(1, sizeof(UpdateStateMessage));
}

void UpdateStateMessage_Destroy(UpdateStateMessage* msg)
{
    free(msg);
}

int UpdateStateMessage_Serialize(UpdateStateMessage* msg, NBN_Stream* stream)
{
    NBN_SerializeUInt(stream, msg->x, 0, GAME_WIDTH);
    NBN_SerializeUInt(stream, msg->y, 0, GAME_HEIGHT);

    NBN_SerializeUInt(stream, msg->missile_count, 0, MAX_MISSILES_CLIENT);

    for (unsigned int i = 0; i < msg->missile_count; i++)
    {

        NBN_SerializeUInt(stream, msg->missiles[i].currentFrame, 0, UINT_MAX);
        NBN_SerializeUInt(stream, msg->missiles[i].framesCounter, 0, UINT_MAX);
        NBN_SerializeUInt(stream, msg->missiles[i].framesSpeed, 0, UINT_MAX);

        NBN_SerializeFloat(stream, msg->missiles[i].rect.x, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        NBN_SerializeFloat(stream, msg->missiles[i].rect.y, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        NBN_SerializeFloat(stream, msg->missiles[i].rect.width, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        NBN_SerializeFloat(stream, msg->missiles[i].rect.height, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);

        NBN_SerializeFloat(stream, msg->missiles[i].pos.x, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        NBN_SerializeFloat(stream, msg->missiles[i].pos.y, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
    }

    return 0;
}

GameStateMessage* GameStateMessage_Create(void)
{
    return (GameStateMessage*)calloc(1, sizeof(GameStateMessage));
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
        NBN_SerializeUInt(stream, msg->client_states[i].x, 0, GAME_WIDTH);
        NBN_SerializeUInt(stream, msg->client_states[i].y, 0, GAME_HEIGHT);

        // Add missile serialization
        NBN_SerializeUInt(stream, msg->client_states[i].missile_count, 0, MAX_MISSILES_CLIENT);

        for (unsigned int j = 0; j < msg->client_states[i].missile_count; j++)
        {
            NBN_SerializeUInt(stream, msg->client_states[i].missiles[j].currentFrame, 0, UINT_MAX);
            NBN_SerializeUInt(stream, msg->client_states[i].missiles[j].framesCounter, 0, UINT_MAX);
            NBN_SerializeUInt(stream, msg->client_states[i].missiles[j].framesSpeed, 0, UINT_MAX);

            NBN_SerializeFloat(stream, msg->client_states[i].missiles[j].rect.x, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
            NBN_SerializeFloat(stream, msg->client_states[i].missiles[j].rect.y, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
            NBN_SerializeFloat(stream, msg->client_states[i].missiles[j].rect.width, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
            NBN_SerializeFloat(stream, msg->client_states[i].missiles[j].rect.height, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);

            NBN_SerializeFloat(stream, msg->client_states[i].missiles[j].pos.x, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
            NBN_SerializeFloat(stream, msg->client_states[i].missiles[j].pos.y, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        }
    }

    // Serialize mobs
    NBN_SerializeUInt(stream, msg->mob_count, 0, MAX_MOBS);

    for (unsigned int i = 0; i < msg->mob_count; i++)
    {
        NBN_SerializeUInt(stream, msg->mobs[i].mob_id, 0, UINT_MAX);
        NBN_SerializeFloat(stream, msg->mobs[i].x, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        NBN_SerializeFloat(stream, msg->mobs[i].y, MIN_FLOAT_VAL, MAX_FLOAT_VAL, 3);
        NBN_SerializeBool(stream, msg->mobs[i].active);
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
