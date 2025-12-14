#include "client.h"

Client::Client()
{
    m_clientInitialized = false;
    m_connected = false;         // Connected to the server
    m_disconnected = false;      // Got disconnected from the server
    m_spawned = false;           // Has spawned
    m_serverCloseCode = 0;       // The server code used when closing the connection
    m_localClientId = 0;
    m_currentScreen = TITLE;
    m_updatedIds = { 0 };
    m_clientCount = 0;
    m_fireMissileKeyPressed = false;
    m_tickDt = 1.0f / TICK_RATE; // Tick delta time (in seconds)
    m_acc = 0;
    m_letterCount = 0;
    m_textBox = { GAME_WIDTH / 2.0f - 170, 180, 365, 50 };
    m_framesCounter = 0;
    m_displayHUD = false;
    m_player = { 0 };
    m_background = { 0 };
    m_localPlayerEntity = 0;

    m_missileAnimationRectangles[0] = { 0, 128, 25, 22 };
    m_missileAnimationRectangles[1] = { 25, 128, 31, 22 };
    m_missileAnimationRectangles[2] = { 56, 128, 40, 22 };
    m_missileAnimationRectangles[3] = { 96, 128, 55, 22 };
    m_missileAnimationRectangles[4] = { 151, 128, 72, 22 };

    memset(m_serverIp, 0, MAX_INPUT_CHARS + 1);

    m_mobBox = { 22, 115, 33, 29 };
}

Client::~Client()
{
    // Unload textures
    UnloadTexture(m_player);
    UnloadTexture(m_background);
    UnloadTexture(m_mob);
    if (m_clientInitialized)
    {
        // Stop the client
        NBN_GameClient_Stop();
    }
    CloseWindow();
}

void Client::InitECS(void)
{
    m_ecsCore.Init();

    // Register all components needed for networked clients
    MiniBuilder::RegisterComponentBuilder registerBuilder;
    registerBuilder.RegisterComponents<
        Position,
        Sprite,
        PlayerSprite,
        AnimationComponent,
        InputController,
        Tag,
        MissileTag,
        playerCooldown,
        NetworkedClient,
        LocalPlayerTag,
        RemotePlayerTag
    >(m_ecsCore);

    // Register systems
    m_clientRendererSystem = m_ecsCore.RegisterSystem<ClientRendererSystem>();
    m_clientRendererSystem->order = 1;
    m_clientRendererSystem->SetPlayerTexture(&m_player);

    // Set system signatures
    Signature clientRendererSignature;
    clientRendererSignature.set(m_ecsCore.GetComponentType<Position>(), true);
    clientRendererSignature.set(m_ecsCore.GetComponentType<Sprite>(), true);
    clientRendererSignature.set(m_ecsCore.GetComponentType<AnimationComponent>(), true);
    clientRendererSignature.set(m_ecsCore.GetComponentType<NetworkedClient>(), true);
    m_ecsCore.SetSystemSignature<ClientRendererSystem>(clientRendererSignature);
}

void Client::SpawnLocalClient(int x, int y, uint32_t client_id)
{
    TraceLog(LOG_INFO, "Received spawn message, position: (%d, %d), client id: %d", x, y, client_id);

    m_localClientId = client_id;

    // Create local player entity using ECS
    m_localPlayerEntity = Prefab::MakeClient(m_ecsCore, (float)x, (float)y, client_id, true, m_player);
    m_clientEntities[client_id] = m_localPlayerEntity;

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
    return m_clientEntities.find(client_id) != m_clientEntities.end();
}

void Client::CreateClient(ClientState state)
{
    TraceLog(LOG_DEBUG, "CreateClient %d", state.client_id);
    assert(m_clientCount < MAX_CLIENTS - 1);

    // Create a new remote client entity using ECS
    Entity clientEntity = Prefab::MakeClient(m_ecsCore, (float)state.x, (float)state.y, state.client_id, false, m_player);
    m_clientEntities[state.client_id] = clientEntity;

    m_clientCount++;

    TraceLog(LOG_INFO, "New remote client (ID: %d)", state.client_id);
}

void Client::UpdateClient(ClientState state)
{
    auto it = m_clientEntities.find(state.client_id);
    if (it == m_clientEntities.end()) {
        TraceLog(LOG_WARNING, "UpdateClient: client_id %d not found", state.client_id);
        return;
    }

    Entity entity = it->second;

    // Update the position component with the latest state from the server
    auto& pos = m_ecsCore.GetComponent<Position>(entity);
    pos.position.x = (float)state.x;
    pos.position.y = (float)state.y;

    // Update missiles for this remote client
    m_remoteMissiles[state.client_id].clear();
    for (unsigned int i = 0; i < state.missile_count; i++)
    {
        m_remoteMissiles[state.client_id].push_back(state.missiles[i]);
    }
}

void Client::DestroyClient(uint32_t client_id)
{
    auto it = m_clientEntities.find(client_id);
    if (it != m_clientEntities.end())
    {
        TraceLog(LOG_INFO, "Destroy disconnected client (ID: %d)", client_id);

        m_ecsCore.DestroyEntity(it->second);
        m_clientEntities.erase(it);
        m_remoteMissiles.erase(client_id); // Clean up missiles for this client
        m_clientCount--;
    }
}

void Client::DestroyDisconnectedClients(void)
{
    /* 
     * Loop over all remote client entities and remove the ones that have not
     * been updated with the last received game state.
     * This is how we detect disconnected clients.
     */
    std::vector<uint32_t> toDestroy;

    for (auto& [client_id, entity] : m_clientEntities)
    {
        // Skip local client
        if (client_id == m_localClientId)
            continue;

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
            toDestroy.push_back(client_id);
    }

    for (uint32_t client_id : toDestroy)
    {
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
        if (state.client_id != m_localClientId)
        {
            // If the client already exists we update it with the latest received state
            if (ClientExists(state.client_id))
                UpdateClient(state);
            else // If the client does not exist, we create it
                CreateClient(state);

            m_updatedIds[i] = state.client_id;
        }
    }

    // Update mobs from server
    m_mobs.clear();
    for (unsigned int i = 0; i < msg->mob_count; i++)
    {
        m_mobs.push_back(msg->mobs[i]);
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

    // Get position from local player entity
    auto& pos = m_ecsCore.GetComponent<Position>(m_localPlayerEntity);

    // Fill message data
    msg->x = (int)pos.position.x;
    msg->y = (int)pos.position.y;
    msg->missile_count = std::min((unsigned int)m_missiles.size(), (unsigned int)MAX_MISSILES_CLIENT);
    memcpy(msg->missiles, m_missiles.data(), msg->missile_count * sizeof(Missile));

    // Unreliably send it to the server
    if (NBN_GameClient_SendUnreliableMessage(UPDATE_STATE_MESSAGE, msg) < 0)
        return -1;

    return 0;
}

int Client::UpdateGameplay(void)
{
    if (!m_spawned)
        return 0;

    // Get local player position component
    auto& pos = m_ecsCore.GetComponent<Position>(m_localPlayerEntity);

    // Firing missile
    if (IsKeyDown(KEY_SPACE) && !m_fireMissileKeyPressed)
    {
        m_fireMissileKeyPressed = true;
        TraceLog(LOG_INFO, "Firing missile");
        Fire();
    }

    if (IsKeyUp(KEY_SPACE))
        m_fireMissileKeyPressed = false;

    // Movement code
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        pos.position.y = MAX(0, pos.position.y - 5);
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        pos.position.y = MIN(GAME_HEIGHT - 50, pos.position.y + 5);

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        pos.position.x = MAX(0, pos.position.x - 5);
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        pos.position.x = MIN(GAME_WIDTH - 50, pos.position.x + 5);

    // Send the latest local client state to the server
    if (SendPositionUpdate() < 0)
    {
        TraceLog(LOG_WARNING, "Failed to send client state update");

        return -1;
    }

    return 0;
}

void Client::DrawClient(Entity entity)
{
    m_clientRendererSystem->RenderClient(m_ecsCore, entity);
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

void Client::DrawGameplay(void)
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

        // Draw all client entities using ECS
        for (auto& [client_id, entity] : m_clientEntities)
        {
            DrawClient(entity);
        }

        // Draw the missiles
        DrawMissiles();

        // Draw the mobs
        DrawMobs();

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
                if (UpdateGameplay() < 0)
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

        DrawGameplay();
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
    m_mob = LoadTexture("resources/sprites/mob_bydo_minions.png");

    // Initialize ECS after textures are loaded
    InitECS();
}

void Client::Run(void)
{
    while (!WindowShouldClose())
    {
        UpdateAndDraw();
    }
}

void Client::Fire(void)
{
    // Get local player position
    auto& playerPos = m_ecsCore.GetComponent<Position>(m_localPlayerEntity);

    Missile missile = {
        .pos = { 0 },
        .rect = { 0 },
        .currentFrame = 0,
        .framesSpeed = 8,
        .framesCounter = 0
    };

    missile.pos.x = playerPos.position.x;
    missile.pos.y = playerPos.position.y;
    missile.rect.x = m_missileAnimationRectangles[missile.currentFrame].x;
    missile.rect.y = m_missileAnimationRectangles[missile.currentFrame].y;
    missile.rect.height = m_missileAnimationRectangles[missile.currentFrame].height;
    missile.rect.width = m_missileAnimationRectangles[missile.currentFrame].width;
    m_missiles.push_back(missile);
}

void Client::DrawMissiles(void)
{
    m_missiles.erase(std::remove_if(m_missiles.begin(), m_missiles.end(),
        [](Missile missile) {
            if (missile.pos.x > GAME_WIDTH)
            {
                TraceLog(LOG_INFO, "Missile out of map... Deleting missile...");
                return true;
            }
            return false;
        }), m_missiles.end());

    for (Missile& missile : m_missiles)
    {
        missile.framesCounter++;

        if (missile.framesCounter >= (TARGET_FPS / missile.framesSpeed))
        {
            missile.framesCounter = 0;
            missile.currentFrame++;
            if (missile.currentFrame > 4) missile.currentFrame = 0;
            missile.rect.x = m_missileAnimationRectangles[missile.currentFrame].x;
            missile.rect.y = m_missileAnimationRectangles[missile.currentFrame].y;
            missile.rect.height = m_missileAnimationRectangles[missile.currentFrame].height;
            missile.rect.width = m_missileAnimationRectangles[missile.currentFrame].width;
        }

        missile.pos.x += 5;

        float frameWidth = missile.rect.width;
        float frameHeight = missile.rect.height;
        Rectangle sourceRec = { missile.rect.x, missile.rect.y, frameWidth, frameHeight };
        Rectangle destRec = { (float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f };
        Vector2 origin = { 0.0f, 0.0f };

        DrawTexturePro(m_player, sourceRec, destRec, origin, 0.0f, WHITE);
    }

    // Draw missiles from remote clients
    for (auto& [client_id, missiles] : m_remoteMissiles)
    {
        for (const Missile& missile : missiles)
        {
            float frameWidth = missile.rect.width;
            float frameHeight = missile.rect.height;
            Rectangle sourceRec = { missile.rect.x, missile.rect.y, frameWidth, frameHeight };
            Rectangle destRec = { (float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(m_player, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
}

void Client::DrawMobs(void)
{
    for (const MobState& mob : m_mobs)
    {
        if (mob.active)
        {
            float frameWidth = m_mobBox.width;
            float frameHeight = m_mobBox.height;
            Rectangle sourceRec = { m_mobBox.x, m_mobBox.y, frameWidth, frameHeight };
            Rectangle destRec = { mob.x, mob.y, (float)MOB_WIDTH, (float)MOB_HEIGHT };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(m_mob, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }
}

int main(int argc, char* argv[])
{
    Client client;

    client.Init();
    client.Run();
    return 0;
}
