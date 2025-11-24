#include "client.h"

// Conversion table between client color values and raylib colors
Color client_colors_to_raylib_colors[] = {
    RED,    // CLI_RED
    LIME,   // CLI_GREEN
    BLUE,   // CLI_BLUE
    YELLOW, // CLI_YELLOW
    ORANGE, // CLI_ORANGE
    PURPLE, // CLI_PURPLE
    PINK    // CLI_PINK
};

Client::Client()
{
    m_clientInitialized = false;
    m_connected = false;         // Connected to the server
    m_disconnected = false;      // Got disconnected from the server
    m_spawned = false;           // Has spawned
    m_serverCloseCode = 0;       // The server code used when closing the connection
    m_currentScreen = TITLE;
    m_clients = {};
    m_updatedIds = {};
    m_clientCount = 0;
    m_colorKeyPressed = false;
    m_localClientState = { 0 };
    m_tickDt = 1.0 / TICK_RATE; // Tick delta time (in seconds)
    m_acc = 0;
    m_letterCount = 0;
    m_textBox = { GAME_WIDTH / 2.0f - 170, 180, 365, 50 };
    m_framesCounter = 0;
    m_displayHUD = false;
    m_player = { 0 };
    m_background = { 0 };

    memset(m_serverIp, 0, MAX_INPUT_CHARS + 1);
}

Client::~Client()
{
    // Unload textures
    UnloadTexture(m_player);
    UnloadTexture(m_background);
    if (m_clientInitialized)
    {
        // Stop the client
        NBN_GameClient_Stop();
    }
    CloseWindow();
}

void Client::SpawnLocalClient(int x, int y, uint32_t client_id)
{
    TraceLog(LOG_INFO, "Received spawn message, position: (%d, %d), client id: %d", x, y, client_id);

    // Update the local client state based on spawn info sent by the server
    m_localClientState.client_id = client_id;
    m_localClientState.x = x;
    m_localClientState.y = y;

    m_spawned = true;
}

void Client::HandleConnection(void)
{
    uint8_t data[32];
    unsigned int data_len = NBN_GameClient_ReadServerData(data);
    NBN_ReadStream rs;

    NBN_ReadStream_Init(&rs, data, data_len);

    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int client_id = 0;

    NBN_SerializeUInt(((NBN_Stream*)&rs), x, 0, GAME_WIDTH);
    NBN_SerializeUInt(((NBN_Stream*)&rs), y, 0, GAME_HEIGHT);
    NBN_SerializeUInt(((NBN_Stream*)&rs), client_id, 0, UINT_MAX);

    SpawnLocalClient(x, y, client_id);

    m_connected = true;
}

void Client::HandleDisconnection(void)
{
    int code = NBN_GameClient_GetServerCloseCode(); // Get the server code used when closing the client connection

    TraceLog(LOG_INFO, "Disconnected from server (code: %d)", code);

    m_disconnected = true;
    m_serverCloseCode = code;
}

bool Client::ClientExists(uint32_t client_id)
{
    for (int i = 0; i < MAX_CLIENTS - 1; i++)
    {
        if (m_clients[i] && m_clients[i]->client_id == client_id)
            return true;
    }

    return false;
}

void Client::CreateClient(ClientState state)
{
    TraceLog(LOG_DEBUG, "CreateClient %d", state.client_id);
    assert(m_clientCount< MAX_CLIENTS - 1);

    ClientState* client = NULL;

    // Create a new remote client state and store it in the remote clients array at the first free slot found
    for (int i = 0; i < MAX_CLIENTS - 1; i++)
    {
        if (m_clients[i] == NULL)
        {
            client = (ClientState*)malloc(sizeof(ClientState));
            m_clients[i] = client;

            break;
        }
    }

    assert(client != NULL);

    // Fill the newly created client state with client state info received from the server
    memcpy(client, &state, sizeof(ClientState));

    m_clientCount++;

    TraceLog(LOG_INFO, "New remote client (ID: %d)", client->client_id);
}

void Client::UpdateClient(ClientState state)
{
    ClientState* client = NULL;

    // Find the client matching the client id of the received remote client state
    for (int i = 0; i < MAX_CLIENTS - 1; i++)
    {
        if (m_clients[i] && m_clients[i]->client_id == state.client_id)
        {
            client = m_clients[i];

            break;
        }
    }

    assert(client != NULL);

    // Update the client state with the latest client state info received from the server
    memcpy(client, &state, sizeof(ClientState));
}

void Client::DestroyClient(uint32_t client_id)
{
    // Find the client matching the client id and destroy it
    for (int i = 0; i < MAX_CLIENTS - 1; i++)
    {
        ClientState* client = m_clients[i];

        if (client && client->client_id == client_id)
        {
            TraceLog(LOG_INFO, "Destroy disconnected client (ID: %d)", client->client_id);

            free(client);
            m_clients[i] = NULL;
            m_clientCount--;

            return;
        }
    }
}

void Client::DestroyDisconnectedClients(void)
{
    /* Loop over all remote client states and remove the one that have not
     * been updated with the last received game state.
     * This is how we detect disconnected clients.
     */
    for (int i = 0; i < MAX_CLIENTS - 1; i++)
    {
        if (m_clients[i] == NULL)
            continue;

        uint32_t client_id = m_clients[i]->client_id;
        bool disconnected = true;

        for (int j = 0; j < MAX_CLIENTS; j++)
        {
            if ((int)client_id == m_updatedIds[j])
            {
                disconnected = false;
                break;
            }
        }

        if (disconnected)
            DestroyClient(client_id);
    }
}

void Client::HandleGameStateMessage(GameStateMessage* msg)
{
    if (!m_spawned)
        return;

    // Start by resetting the updated client ids array
    for (int i = 0; i < MAX_CLIENTS; i++)
        m_updatedIds[i] = -1;

    // Loop over the received client states
    for (unsigned int i = 0; i < msg->client_count; i++)
    {
        ClientState state = msg->client_states[i];

        // Ignore the state of the local client
        if (state.client_id != m_localClientState.client_id)
        {
            // If the client already exists we update it with the latest received state
            if (ClientExists(state.client_id))
                UpdateClient(state);
            else // If the client does not exist, we create it
                CreateClient(state);

            m_updatedIds[i] = state.client_id;
        }
    }

    // Destroy disconnected clients
    DestroyDisconnectedClients();

    GameStateMessage_Destroy(msg);
}

void Client::HandleReceivedMessage(void)
{
    // Fetch info about the last received message
    NBN_MessageInfo msg_info = NBN_GameClient_GetMessageInfo();

    switch (msg_info.type)
    {
        // We received the latest game state from the server
    case GAME_STATE_MESSAGE:
        HandleGameStateMessage((GameStateMessage*)msg_info.data);
        break;
    }
}

void Client::HandleGameClientEvent(int ev)
{
    switch (ev)
    {
    case NBN_CONNECTED:
        // We are connected to the server
        HandleConnection();
        break;

    case NBN_DISCONNECTED:
        // The server has closed our connection
        HandleDisconnection();
        break;

    case NBN_MESSAGE_RECEIVED:
        // We received a message from the server
        HandleReceivedMessage();
        break;
    }
}

int Client::SendPositionUpdate(void)
{
    UpdateStateMessage* msg = UpdateStateMessage_Create();

    // Fill message data
    msg->x = m_localClientState.x;
    msg->y = m_localClientState.y;
    msg->val = m_localClientState.val;

    // Unreliably send it to the server
    if (NBN_GameClient_SendUnreliableMessage(UPDATE_STATE_MESSAGE, msg) < 0)
        return -1;

    return 0;
}

int Client::SendColorUpdate(void)
{
    ChangeColorMessage* msg = ChangeColorMessage_Create();

    // Fill message data
    msg->color = m_localClientState.color;

    // Reliably send it to the server
    if (NBN_GameClient_SendReliableMessage(CHANGE_COLOR_MESSAGE, msg) < 0)
        return -1;

    return 0;
}

int Client::Update(void)
{
    if (!m_spawned)
        return 0;

    // Movement code
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        m_localClientState.y = MAX(0, m_localClientState.y - 5);
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        m_localClientState.y = MIN(GAME_HEIGHT - 50, m_localClientState.y + 5);

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        m_localClientState.x = MAX(0, m_localClientState.x - 5);
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        m_localClientState.x = MIN(GAME_WIDTH - 50, m_localClientState.x + 5);

    // Color switching
    if (IsKeyDown(KEY_SPACE) && !m_colorKeyPressed)
    {
        m_colorKeyPressed = true;
        m_localClientState.color = (ClientColor)((m_localClientState.color + 1) % MAX_COLORS);

        TraceLog(LOG_INFO, "Switched color, new color: %d", m_localClientState.color);

        if (SendColorUpdate() < 0)
        {
            TraceLog(LOG_WARNING, "Failed to send color update");

            return -1;
        }
    }

    if (IsKeyUp(KEY_SPACE))
        m_colorKeyPressed = false;

    // Increasing/Decreasing floating point value
    if (IsKeyDown(KEY_K))
        m_localClientState.val = MIN(MAX_FLOAT_VAL, m_localClientState.val + 0.005);

    if (IsKeyDown(KEY_J))
        m_localClientState.val = MAX(MIN_FLOAT_VAL, m_localClientState.val - 0.005);

    // Send the latest local client state to the server
    if (SendPositionUpdate() < 0)
    {
        TraceLog(LOG_WARNING, "Failed to send client state update");

        return -1;
    }

    return 0;
}

void Client::DrawClient(ClientState* state, bool is_local)
{
    float frameWidth = 32;
    float frameHeight = 22.0f;
    Rectangle sourceRec = { 0.0f, 30.0f, frameWidth, frameHeight };
    Rectangle rec = { (float)state->x, (float)state->y, frameWidth * 2.0f, frameHeight * 2.0f };
    Rectangle destRec = { (float)state->x, (float)state->y, frameWidth * 2.0f, frameHeight * 2.0f};
    Vector2 origin = { 0.0f, 0.0f };

    // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
    // source_rect defines the part of the texture we use for drawing
    // dest_rect defines the rectangle where our texture part will fit (scaling it to fit)
    // origin defines the point of the texture used as reference for rotation and scaling
    // rotation defines the texture rotation (using origin as rotation point)
    DrawTexturePro(m_player, sourceRec, destRec, origin, 0.0f, WHITE);
    if (is_local)
        DrawRectangleLinesEx(rec, 3, DARKBROWN);
}

void Client::DrawHUD(void)
{
    NBN_ConnectionStats stats = NBN_GameClient_GetStats();
    unsigned int ping = stats.ping * 1000;
    unsigned int packet_loss = stats.packet_loss * 100;

    DrawText(TextFormat("FPS: %d", GetFPS()), 450, 350, 32, MAROON);
    DrawText(TextFormat("Ping: %d ms", ping), 450, 400, 32, MAROON);
    DrawText(TextFormat("Packet loss: %d %%", packet_loss), 450, 450, 32, MAROON);
    DrawText(TextFormat("Upload: %.1f Bps", stats.upload_bandwidth), 450, 500, 32, MAROON);
    DrawText(TextFormat("Download: %.1f Bps", stats.download_bandwidth), 450, 550, 32, MAROON);
}

void Client::DrawBackground(void)
{
    float frameWidth = GAME_WIDTH / 4;
    float frameHeight = GAME_HEIGHT / 4;
    Rectangle source_rect = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };
    Rectangle dest_rect = { 0.0f , 0.0f, GAME_WIDTH, GAME_HEIGHT};
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(m_background, source_rect, dest_rect, origin, 0.0f, WHITE);
}

void Client::Draw(void)
{
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    if (m_disconnected)
    {
        if (m_serverCloseCode == -1)
        {
            if (m_connected)
                DrawText("Connection to the server was lost", 265, 280, 20, RED);
            else
                DrawText("Server cannot be reached", 265, 280, 20, RED);
        }
        else if (m_serverCloseCode == SERVER_FULL_CODE)
        {
            DrawText("Cannot connect, server is full", 265, 280, 20, RED);
        }
    }
    else if (m_connected && m_spawned)
    {
        // Start by drawing the background
        DrawBackground();

        // Draw the remote clients
        for (int i = 0; i < MAX_CLIENTS - 1; i++)
        {
            if (m_clients[i])
                DrawClient(m_clients[i], false);
        }

        // Then draw the local client
        DrawClient(&m_localClientState, true);

        // Draw hud if m_hudDisplay variable is true
        if (m_displayHUD)
        {
            DrawHUD();
        }
    }
    else
    {
        DrawText("Connecting to server...", 265, 280, 20, RED);
    }

    EndDrawing();
}

void Client::UpdateAndDraw(void)
{
    switch (m_currentScreen)
    {
    case TITLE:
    {
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        {
            m_currentScreen = IP_ADDRESS;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("R-Type", 190, 200, 20, DARKGRAY);

        EndDrawing();
    } break;
    case IP_ADDRESS:
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitClient(m_serverIp);
            m_currentScreen = GAMEPLAY;
        }

        // Set the window's cursor to the I-Beam
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0)
        {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (m_letterCount < MAX_INPUT_CHARS))
            {
                m_serverIp[m_letterCount] = (char)key;
                m_serverIp[m_letterCount + 1] = '\0'; // Add null terminator at the end of the string
                m_letterCount++;
            }

            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            m_letterCount--;
            if (m_letterCount < 0) m_letterCount = 0;
            m_serverIp[m_letterCount] = '\0';
        }

        m_framesCounter++;

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawRectangleRec(m_textBox, LIGHTGRAY);
        DrawRectangleLines((int)m_textBox.x, (int)m_textBox.y, (int)m_textBox.width, (int)m_textBox.height, RED);

        DrawText(m_serverIp, (int)m_textBox.x + 5, (int)m_textBox.y + 8, 40, MAROON);

        DrawText(TextFormat("INPUT CHARS: %i/%i", m_letterCount, MAX_INPUT_CHARS), 315, 250, 20, DARKGRAY);

        if (m_letterCount < MAX_INPUT_CHARS)
        {
            // Draw blinking underscore char
            if (((m_framesCounter / 20) % 2) == 0) DrawText("_", (int)m_textBox.x + 8 + MeasureText(m_serverIp, 40), (int)m_textBox.y + 12, 40, MAROON);
        }
        else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);

        EndDrawing();
    } break;
    case GAMEPLAY:
    {
        // Very basic fixed timestep implementation.
        // Target FPS is 100 but the simulation runs at
        // TICK_RATE ticks per second.
        //
        // We keep track of accumulated times and simulates as many tick as we can using that time

        m_acc += GetFrameTime(); // Accumulates time

        // Simulates as many ticks as we can
        while (m_acc >= m_tickDt)
        {
            int ev;

            while ((ev = NBN_GameClient_Poll()) != NBN_NO_EVENT)
            {
                if (ev < 0)
                {
                    TraceLog(LOG_WARNING, "An occured while polling network events. Exit");

                    break;
                }

                HandleGameClientEvent(ev);
            }

            if (m_connected && !m_disconnected)
            {
                if (Update() < 0)
                    break;
            }

            if (!m_disconnected)
            {
                if (NBN_GameClient_SendPackets() < 0)
                {
                    TraceLog(LOG_ERROR, "An occured while flushing the send queue. Exit");

                    break;
                }
            }

            m_acc -= m_tickDt; // Consumes time
        }

        Draw();
    } break;
    default: break;
    }
}

void Client::InitClient(char* serverIp)
{
    NBN_UDP_Register();

    // Initialize the client with a protocol name (must be the same than the one used by the server), the server ip address and port

    // Start the client with a protocol name (must be the same than the one used by the server)
    // the server host and port
    if (NBN_GameClient_StartEx(PROTOCOL_NAME, serverIp, PORT, NULL, 0) < 0)
    {
        TraceLog(LOG_WARNING, "Game client failed to start. Exit");

        return;
    }

    // Register messages, have to be done after NBN_GameClient_StartEx
    // Messages need to be registered on both client and server side
    NBN_GameClient_RegisterMessage(
        CHANGE_COLOR_MESSAGE,
        (NBN_MessageBuilder)ChangeColorMessage_Create,
        (NBN_MessageDestructor)ChangeColorMessage_Destroy,
        (NBN_MessageSerializer)ChangeColorMessage_Serialize);
    NBN_GameClient_RegisterMessage(
        UPDATE_STATE_MESSAGE,
        (NBN_MessageBuilder)UpdateStateMessage_Create,
        (NBN_MessageDestructor)UpdateStateMessage_Destroy,
        (NBN_MessageSerializer)UpdateStateMessage_Serialize);
    NBN_GameClient_RegisterMessage(
        GAME_STATE_MESSAGE,
        (NBN_MessageBuilder)GameStateMessage_Create,
        (NBN_MessageDestructor)GameStateMessage_Destroy,
        (NBN_MessageSerializer)GameStateMessage_Serialize);

    // Network conditions simulated variables (read from the command line, default is always 0)
    NBN_GameClient_SetPing(GetOptions().ping);
    NBN_GameClient_SetJitter(GetOptions().jitter);
    NBN_GameClient_SetPacketLoss(GetOptions().packet_loss);
    NBN_GameClient_SetPacketDuplication(GetOptions().packet_duplication);

    m_clientInitialized = true;
}

void Client::Init(void)
{
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(GAME_WIDTH, GAME_HEIGHT, "R-Type");
    SetTargetFPS(TARGET_FPS);

    // Load textures
    m_player = LoadTexture("resources/sprites/player_r-9c_war-head.png");
    m_background = LoadTexture("resources/sprites/space_background.png");
}

void Client::Run(void)
{
    while (!WindowShouldClose())
    {
        UpdateAndDraw();
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::unique_ptr<Client> client = std::make_unique<Client>();

    client->Init();
    client->Run();
    return 0;
}
