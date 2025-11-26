#include "server.h"

Server* g_serverInstance = nullptr;

static void SigintHandler(int dummy)
{
    g_serverInstance->running = false;
}

Server::Server()
{
    g_serverInstance = this; // Set global instance pointer
    signal(SIGINT, SigintHandler);

    m_clients = {};
    m_spawns = {
        {50, 50},
        {GAME_WIDTH - 100, 50},
        {50, GAME_HEIGHT - 100},
        {GAME_WIDTH - 100, GAME_HEIGHT - 100}
    };

    float tick_dt = 1.f / TICK_RATE; // Tick delta time
}

Server::~Server()
{
    // Stop the server
    NBN_GameServer_Stop();
}

void Server::AcceptConnection(unsigned int x, unsigned int y, NBN_ConnectionHandle conn)
{
    NBN_WriteStream ws;
    uint8_t data[32];

    NBN_WriteStream_Init(&ws, data, sizeof(data));

    NBN_SerializeUInt((NBN_Stream*)&ws, x, 0, GAME_WIDTH);
    NBN_SerializeUInt((NBN_Stream*)&ws, y, 0, GAME_HEIGHT);
    NBN_SerializeUInt((NBN_Stream*)&ws, conn, 0, UINT_MAX);

    // Accept the connection
    NBN_GameServer_AcceptIncomingConnectionWithData(data, sizeof(data));
}

int Server::HandleNewConnection(void)
{
    TraceLog(LOG_INFO, "New connection");

    // If the server is full
    if (m_clientCount== MAX_CLIENTS)
    {
        // Reject the connection (send a SERVER_FULL_CODE code to the client)
        TraceLog(LOG_INFO, "Connection rejected");
        NBN_GameServer_RejectIncomingConnectionWithCode(SERVER_FULL_CODE);

        return 0;
    }

    // Otherwise...

    NBN_ConnectionHandle client_handle;

    client_handle = NBN_GameServer_GetIncomingConnection();

    // Get a spawning position for the client
    Vector2 spawn = m_spawns[client_handle % MAX_CLIENTS];

    // Build some "initial" data that will be sent to the connected client

    unsigned int x = (unsigned int)spawn.x;
    unsigned int y = (unsigned int)spawn.y;

    AcceptConnection(x, y, client_handle);

    TraceLog(LOG_INFO, "Connection accepted (ID: %d)", client_handle);

    Client* client = NULL;

    // Find a free slot in the clients array and create a new client
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (m_clients[i] == NULL)
        {
            client = (Client*)malloc(sizeof(Client));
            m_clients[i] = client;

            break;
        }
    }

    assert(client != NULL);

    client->client_handle = client_handle; // Store the nbnet connection ID

    // Fill the client state with initial spawning data
    client->state.client_id = client_handle;
    client->state.x = 200;
    client->state.y = 400;
    client->state.color = CLI_RED;
    client->state.val = 0;

    m_clientCount++;

    return 0;
}

Client* Server::FindClientById(uint32_t client_id)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (m_clients[i] && m_clients[i]->state.client_id == client_id)
            return m_clients[i];
    }

    return NULL;
}

void Server::DestroyClient(Client* client)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (m_clients[i] && m_clients[i]->state.client_id == client->state.client_id)
        {
            m_clients[i] = NULL;

            return;
        }
    }

    free(client);
}

void Server::HandleClientDisconnection()
{
    NBN_ConnectionHandle client_handle = NBN_GameServer_GetDisconnectedClient(); // Get the disconnected client

    TraceLog(LOG_INFO, "Client has disconnected (id: %d)", client_handle);

    Client* client = FindClientById(client_handle);

    assert(client);

    DestroyClient(client);

    m_clientCount--;
}

void Server::HandleUpdateStateMessage(UpdateStateMessage * msg, Client * sender)
{
    // Update the state of the client with the data from the received UpdateStateMessage message
    sender->state.x = msg->x;
    sender->state.y = msg->y;
    sender->state.val = msg->val;

    UpdateStateMessage_Destroy(msg);
}

void Server::HandleChangeColorMessage(ChangeColorMessage* msg, Client* sender)
{
    // Update the client color
    sender->state.color = msg->color;

    ChangeColorMessage_Destroy(msg);
}

void Server::HandleReceivedMessage(void)
{
    // Fetch info about the last received message
    NBN_MessageInfo msg_info = NBN_GameServer_GetMessageInfo();

    // Find the client that sent the message
    Client* sender = FindClientById(msg_info.sender);

    assert(sender != NULL);

    switch (msg_info.type)
    {
    case UPDATE_STATE_MESSAGE:
        // The server received a client state update
        HandleUpdateStateMessage((UpdateStateMessage*)msg_info.data, sender);
        break;

    case CHANGE_COLOR_MESSAGE:
        // The server received a client switch color action
        HandleChangeColorMessage((ChangeColorMessage*)msg_info.data, sender);
        break;
    }
}

int Server::HandleGameServerEvent(int ev)
{
    switch (ev)
    {
    case NBN_NEW_CONNECTION:
        // A new client has requested a connection
        if (HandleNewConnection() < 0)
            return -1;
        break;

    case NBN_CLIENT_DISCONNECTED:
        // A previously connected client has disconnected
        HandleClientDisconnection();
        break;

    case NBN_CLIENT_MESSAGE_RECEIVED:
        // A message from a client has been received
        HandleReceivedMessage();
        break;
    }

    return 0;
}

// Broadcasts the latest game state to all connected clients
int Server::BroadcastGameState(void)
{
    ClientState client_states[MAX_CLIENTS];
    unsigned int client_index = 0;

    // Loop over the clients and build an array of ClientState
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        Client* client = m_clients[i];

        if (client == NULL) continue;

        client_states[client_index].client_id = client->state.client_id;
        client_states[client_index].x = client->state.x;
        client_states[client_index].y = client->state.y;
        client_states[client_index].val = client->state.val;
        client_states[client_index].color = client->state.color;
        client_index++;
    }

    assert(client_index == m_clientCount);

    // Broadcast the game state to all clients
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        Client* client = m_clients[i];

        if (client == NULL) continue;

        GameStateMessage* msg = GameStateMessage_Create();

        // Fill message data
        msg->client_count = m_clientCount;
        memcpy(msg->client_states, client_states, sizeof(ClientState) * m_clientCount);

        // Unreliably send the message to all connected clients
        NBN_GameServer_SendUnreliableMessageTo(client->client_handle, GAME_STATE_MESSAGE, msg);

        // TraceLog(LOG_DEBUG, "Sent game state to client %d (%d, %d)", client->client_id, client_count, client_index);
    }

    return 0;
}

void Server::Run(void)
{
    while (running)
    {
        int ev;

        // Poll for server events
        while ((ev = NBN_GameServer_Poll()) != NBN_NO_EVENT)
        {
            if (ev < 0)
            {
                TraceLog(LOG_ERROR, "An occured while polling network events. Exit");

                break;
            }

            if (HandleGameServerEvent(ev) < 0)
                break;
        }

        // Broadcast latest game state
        if (BroadcastGameState() < 0)
        {
            TraceLog(LOG_ERROR, "An occured while broadcasting game states. Exit");

            break;
        }

        // Pack all enqueued messages as packets and send them
        if (NBN_GameServer_SendPackets() < 0)
        {
            TraceLog(LOG_ERROR, "An occured while flushing the send queue. Exit");

            break;
        }

        NBN_GameServerStats stats = NBN_GameServer_GetStats();

        TraceLog(LOG_TRACE, "Upload: %f Bps | Download: %f Bps", stats.upload_bandwidth, stats.download_bandwidth);

        // Cap the simulation rate to TICK_RATE ticks per second (just like for the client)
#if defined(_WIN32) || defined(_WIN64)
        Sleep(tick_dt * 1000);
#else
        long nanos = tick_dt * 1e9;
        struct timespec t = { .tv_sec = nanos / 999999999, .tv_nsec = nanos % 999999999 };

        nanosleep(&t, &t);
#endif
    }
}

void Server::Init(int argc, char **argv)
{
    // Read command line arguments
    if (ReadCommandLine(argc, argv))
    {
        printf("Usage: server [--packet_loss=<value>] [--packet_duplication=<value>] [--ping=<value>] \
                [--jitter=<value>]\n");
        return;
    }

    // Even though we do not display anything we still use raylib logging capacibilities
    SetTraceLogLevel(LOG_DEBUG);

    NBN_UDP_Register(); // Register the UDP driver

    // Start the server with a protocol name and a port
    if (NBN_GameServer_StartEx(PROTOCOL_NAME, PORT) < 0)
    {
        TraceLog(LOG_ERROR, "Game server failed to start. Exit");

        return;
    }

    // Register messages, have to be done after NBN_GameServer_StartEx
    NBN_GameServer_RegisterMessage(
        CHANGE_COLOR_MESSAGE,
        (NBN_MessageBuilder)ChangeColorMessage_Create,
        (NBN_MessageDestructor)ChangeColorMessage_Destroy,
        (NBN_MessageSerializer)ChangeColorMessage_Serialize);
    NBN_GameServer_RegisterMessage(
        UPDATE_STATE_MESSAGE,
        (NBN_MessageBuilder)UpdateStateMessage_Create,
        (NBN_MessageDestructor)UpdateStateMessage_Destroy,
        (NBN_MessageSerializer)UpdateStateMessage_Serialize);
    NBN_GameServer_RegisterMessage(
        GAME_STATE_MESSAGE,
        (NBN_MessageBuilder)GameStateMessage_Create,
        (NBN_MessageDestructor)GameStateMessage_Destroy,
        (NBN_MessageSerializer)GameStateMessage_Serialize);

    // Network conditions simulated variables (read from the command line, default is always 0)
    NBN_GameServer_SetPing(GetOptions().ping);
    NBN_GameServer_SetJitter(GetOptions().jitter);
    NBN_GameServer_SetPacketLoss(GetOptions().packet_loss);
    NBN_GameServer_SetPacketDuplication(GetOptions().packet_duplication);

    tick_dt = 1.f / TICK_RATE; // Tick delta time
}

int main(int argc, char* argv[])
{
    std::unique_ptr<Server> server = std::make_unique<Server>();

    server->Init(argc, argv);
    server->Run();
    return 0;
}
