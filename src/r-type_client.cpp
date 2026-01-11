#include "RoarEngine.h"
#include "r-type.h"
#include <algorithm>

typedef enum { TITLE = 0, IP_ADDRESS, GAMEPLAY } GameScreen;

constexpr auto MAX_INPUT_CHARS = 15;
constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 600;

void InitClient(char *serverIp);
void DrawMissiles(void);
void Fire(void);

struct {
    // Mob storage (received from server)
    std::vector<MobState> m_mobs;

    // Wave system info (received from server)
    float m_countdownTimer;
    unsigned int m_currentWave;
    bool m_waveActive;

    // Networking
    bool m_clientInitialized;
    bool m_connected;              // Connected to the server
    bool m_disconnected;           // Got disconnected from the server
    bool m_spawned;                // Has spawned
    int m_serverCloseCode;         // The server code used when closing the connection
    uint32_t m_localClientId;      // Local client ID from server
    unsigned int m_heartbeatTimer; // Timer for sending heartbeats

    ClientState m_localClientState; // The state of the local client

    GameScreen m_currentScreen;
    std::vector<int> m_updatedIds;
    std::vector<Rectangle> m_missileAnimationRectangles;
    std::vector<Missile> m_missiles;
    std::vector<ClientState *> m_clients;
    unsigned int m_clientCount;
    bool m_fireMissileKeyPressed;
    double m_tickDt; // Tick delta time (in seconds)
    double m_acc;
    char m_serverIp[MAX_INPUT_CHARS + 1]; // NOTE: One extra space required for null terminator char '\0'
    int m_letterCount;
    Rectangle m_textBox;
    int m_framesCounter;
    bool m_displayHUD;
    Texture2D m_player;
    Texture2D m_background;
    Texture2D m_mob;
    Rectangle m_mobBox;
} state;

Roar::NetlibNetwork *net = nullptr;
Roar::NetClient *client = nullptr;

void init() {
    Roar::PluginSystem::AddPlugin("networking");
    Roar::PluginSystem::Startup();

    net = Roar::registry->GetSystem<Roar::NetlibNetwork>("NetlibNetwork");
    RO_LOG_INFO("Network plugin ID: {}", net->GetID());

    client = net->NewClient();

    state.m_clientInitialized = false;
    state.m_connected = false;
    state.m_disconnected = false;
    state.m_spawned = false;
    state.m_serverCloseCode = 0;
    state.m_localClientId = 0;
    state.m_heartbeatTimer = 0;
    state.m_currentScreen = TITLE;
    state.m_updatedIds = {0};
    state.m_clientCount = 0;
    state.m_fireMissileKeyPressed = false;
    state.m_tickDt = 1.0f / TICK_RATE;
    state.m_acc = 0;
    state.m_letterCount = 0;
    state.m_textBox = {GAME_WIDTH / 2.0f - 170, 180, 365, 50};
    state.m_framesCounter = 0;
    state.m_displayHUD = false;
    state.m_player = {0};
    state.m_background = {0};
    state.m_mob = {0};
    // Wave system
    state.m_countdownTimer = 0.0f;
    state.m_currentWave = 0;
    state.m_waveActive = false;
    state.m_missileAnimationRectangles.push_back({0, 128, 25, 22});
    state.m_missileAnimationRectangles.push_back({25, 128, 31, 22});
    state.m_missileAnimationRectangles.push_back({56, 128, 40, 22});
    state.m_missileAnimationRectangles.push_back({96, 128, 55, 22});
    state.m_missileAnimationRectangles.push_back({151, 128, 72, 22});

    state.m_clients.push_back(nullptr);
    state.m_clients.push_back(nullptr);
    state.m_clients.push_back(nullptr);
    state.m_clients.push_back(nullptr);

    memset(state.m_serverIp, 0, MAX_INPUT_CHARS + 1);

    SetTraceLogLevel(LOG_DEBUG);
    SetTargetFPS(100);

    state.m_player = LoadTexture("resources/sprites/player_r-9c_war-head.png");
    state.m_background = LoadTexture("resources/sprites/space_background.png");
    state.m_mob = LoadTexture("resources/sprites/mob_bydo_minions.png");
}

void SpawnLocalClient(int x, int y, uint32_t client_id) {
    TraceLog(LOG_INFO, "Received spawn message, position: (%d, %d), client id: %d", x, y, client_id);

    state.m_localClientId = client_id;
    state.m_spawned = true;
}

void HandleConnectAccept(Roar::NetBuffer &buffer) {
    ConnectAcceptData data = DeserializeConnectAcceptData(buffer);

    TraceLog(LOG_INFO, "Connection accepted by server");

    SpawnLocalClient(data.spawn_x, data.spawn_y, data.client_id);
    state.m_connected = true;
}

void HandleConnectReject(Roar::NetBuffer &buffer) {
    int code = buffer.ReadInt32();

    TraceLog(LOG_INFO, "Connection rejected by server (code: %d)", code);

    state.m_disconnected = true;
    state.m_serverCloseCode = code;
}

void CreateClient(ClientState client_state) {
    TraceLog(LOG_DEBUG, "CreateClient %d", client_state.client_id);
    assert(state.m_clientCount < MAX_CLIENTS - 1);

    state.m_clientCount++;
    TraceLog(LOG_INFO, "New remote client (ID: %d)", client_state.client_id);
}

bool ClientExists(uint32_t client_id) {
    for (int i = 0; i < MAX_CLIENTS - 1; i++) {
        if (state.m_clients[i] && state.m_clients[i]->client_id == client_id)
            return true;
    }

    return false;
}

void UpdateClient(ClientState client_state) {
    ClientState *client = NULL;

    // Find the client matching the client id of the received remote client state
    for (int i = 0; i < MAX_CLIENTS - 1; i++) {
        if (state.m_clients[i] && state.m_clients[i]->client_id == client_state.client_id) {
            client = state.m_clients[i];
            break;
        }
    }

    assert(client != NULL);

    // Update the client state with the latest client state info received from the server
    memcpy(client, &client_state, sizeof(ClientState));
}

void DestroyClient(uint32_t client_id) {
    /*
     * Loop over all remote client states and remove the one that have not
     * been updated with the last received game state.
     * This is how we detect disconnected clients.
     */
    for (int i = 0; i < MAX_CLIENTS - 1; i++) {
        if (state.m_clients[i] == NULL)
            continue;

        uint32_t client_id = state.m_clients[i]->client_id;
        bool disconnected = true;

        for (int j = 0; j < MAX_CLIENTS; j++) {
            if ((int)client_id == state.m_updatedIds[j]) {
                disconnected = false;
                break;
            }
        }

        if (disconnected)
            DestroyClient(client_id);
    }
}

void DestroyDisconnectedClients(void) {}

void HandleGameStateMessage(GameStateMessage *msg) {
    if (!state.m_spawned)
        return;

    // Start by resetting the updated client ids array
    for (int i = 0; i < MAX_CLIENTS; i++)
        state.m_updatedIds[i] = -1;

    // Loop over the received client states
    for (unsigned int i = 0; i < msg->client_count; i++) {
        ClientState client_state = msg->client_states[i];

        // Ignore the state of the local client
        if (client_state.client_id != state.m_localClientState.client_id) {
            // If the client already exists we update it with the latest received state
            if (ClientExists(client_state.client_id))
                UpdateClient(client_state);
            else // If the client does not exist, we create it
                CreateClient(client_state);

            state.m_updatedIds[i] = client_state.client_id;
        }
    }

    // Destroy disconnected clients
    DestroyDisconnectedClients();
}

void HandleGameStateMessage(Roar::NetBuffer &buffer) {
    if (!state.m_spawned)
        return;

    GameStateMessage msg = DeserializeGameStateMessage(buffer);

    for (int i = 0; i < MAX_CLIENTS; i++)
        state.m_updatedIds[i] = -1;

    for (unsigned int i = 0; i < msg.client_count; i++) {
        ClientState client_state = msg.client_states[i];

        if (client_state.client_id != state.m_localClientId) {
            if (ClientExists(client_state.client_id))
                UpdateClient(client_state);
            else
                CreateClient(client_state);

            state.m_updatedIds[i] = client_state.client_id;
        }
    }

    DestroyDisconnectedClients();
}

void HandleReceivedMessages(void) {
    Roar::NetBuffer buffer;

    while (client->Receive(buffer) > 0) {
        if (!buffer.CanRead(1)) {
            buffer.Clear();
            continue;
        }

        uint8_t msgType = buffer.ReadUInt8();

        switch (msgType) {
        case MSG_CONNECT_ACCEPT:
            HandleConnectAccept(buffer);
            break;

        case MSG_CONNECT_REJECT:
            HandleConnectReject(buffer);
            break;

        case MSG_GAME_STATE:
            HandleGameStateMessage(buffer);
            break;
        }

        buffer.Clear();
    }
}

int SendConnectRequest(void) {
    Roar::NetBuffer buffer;
    buffer.WriteUInt8(MSG_CONNECT_REQUEST);

    if (!client->Send(buffer)) {
        TraceLog(LOG_WARNING, "Failed to send connect request");
        return -1;
    }

    return 0;
}

int SendPositionUpdate(void) {
    if (!state.m_connected || state.m_disconnected)
        return 0;

    Roar::NetBuffer buffer;
    buffer.WriteUInt8(MSG_UPDATE_STATE);

    UpdateStateMessage msg;
    msg.x = state.m_localClientState.x;
    msg.y = state.m_localClientState.y;
    msg.missile_count = std::min((unsigned int)state.m_missiles.size(), (unsigned int)MAX_MISSILES_CLIENT);
    memcpy(msg.missiles, state.m_missiles.data(), msg.missile_count * sizeof(Missile));

    SerializeUpdateStateMessage(buffer, msg);

    if (!client->Send(buffer))
        return -1;

    return 0;
}

void SendHeartbeat(void) {
    if (!state.m_connected || state.m_disconnected)
        return;

    Roar::NetBuffer buffer;
    buffer.WriteUInt8(MSG_HEARTBEAT);
    client->Send(buffer);
}

int UpdateGameplay(void) {
    if (!state.m_spawned)
        return 0;

    // Firing missile
    if (IsKeyDown(KEY_SPACE) && !state.m_fireMissileKeyPressed) {
        state.m_fireMissileKeyPressed = true;
        TraceLog(LOG_INFO, "Firing missile");
        Fire();
    }

    if (IsKeyUp(KEY_SPACE))
        state.m_fireMissileKeyPressed = false;

    // Movement code
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        state.m_localClientState.y = std::max(0, state.m_localClientState.y - 5);
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        state.m_localClientState.y = std::min(GAME_HEIGHT - 50, state.m_localClientState.y + 5);

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        state.m_localClientState.x = std::max(0, state.m_localClientState.x - 5);
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        state.m_localClientState.x = std::min(GAME_WIDTH - 50, state.m_localClientState.x + 5);

    // Send the latest local client state to the server
    if (SendPositionUpdate() < 0) {
        TraceLog(LOG_WARNING, "Failed to send client state update");
        return -1;
    }

    return 0;
}

void DrawClient(ClientState *client_state, bool is_local) {
    float frameWidth = 32;
    float frameHeight = 22.0f;
    Rectangle sourceRec = {0.0f, 30.0f, frameWidth, frameHeight};
    Rectangle rec = {(float)client_state->x, (float)client_state->y, frameWidth * 2.0f, frameHeight * 2.0f};
    Rectangle destRec = {(float)client_state->x, (float)client_state->y, frameWidth * 2.0f, frameHeight * 2.0f};
    Vector2 origin = {0.0f, 0.0f};

    // NOTE: Using DrawTexturePro() we can easily rotate and scale the part of the texture we draw
    // source_rect defines the part of the texture we use for drawing
    // dest_rect defines the rectangle where our texture part will fit (scaling it to fit)
    // origin defines the point of the texture used as reference for rotation and scaling
    // rotation defines the texture rotation (using origin as rotation point)
    DrawTexturePro(state.m_player, sourceRec, destRec, origin, 0.0f, WHITE);
    if (is_local)
        DrawRectangleLinesEx(rec, 3, DARKBROWN);
}

void DrawHUD(void) {
    DrawText(TextFormat("FPS: %d", GetFPS()), 450, 350, 32, MAROON);
    DrawText(TextFormat("Client ID: %d", state.m_localClientId), 450, 400, 32, MAROON);
    DrawText(TextFormat("Connected: %s", state.m_connected ? "Yes" : "No"), 450, 450, 32, MAROON);
}

void DrawBackground(void) {}

void DrawGameplay(void) {
    // Update parallax animation
    float deltaTime = GetFrameTime();

    ClearBackground(BLACK);

    if (state.m_disconnected) {
        if (state.m_serverCloseCode == SERVER_FULL_CODE) {
            DrawText("Cannot connect, server is full", 265, 280, 20, RED);
        } else {
            DrawText("Connection to the server was lost", 265, 280, 20, RED);
        }
    } else if (state.m_connected && state.m_spawned) {
        DrawBackground();

        // Draw the remote clients
        for (int i = 0; i < MAX_CLIENTS - 1; i++) {
            if (state.m_clients[i])
                DrawClient(state.m_clients[i], false);
        }

        DrawMissiles();

        if (state.m_displayHUD) {
            DrawHUD();
        }
    } else {
        DrawText("Connecting to server...", 265, 280, 20, RED);
    }
}

void UpdateAndDraw(void) {
    switch (state.m_currentScreen) {
    case TITLE: {
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
            state.m_currentScreen = IP_ADDRESS;
        }

        DrawText("R-Type", 190, 200, 20, DARKGRAY);
    } break;

    case IP_ADDRESS: {
        if (IsKeyPressed(KEY_ENTER)) {
            InitClient(state.m_serverIp);
            state.m_currentScreen = GAMEPLAY;
        }

        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();

        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (state.m_letterCount < MAX_INPUT_CHARS)) {
                state.m_serverIp[state.m_letterCount] = (char)key;
                state.m_serverIp[state.m_letterCount + 1] = '\0';
                state.m_letterCount++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            state.m_letterCount--;
            if (state.m_letterCount < 0)
                state.m_letterCount = 0;
            state.m_serverIp[state.m_letterCount] = '\0';
        }

        state.m_framesCounter++;

        ClearBackground(RAYWHITE);

        DrawRectangleRec(state.m_textBox, LIGHTGRAY);
        DrawRectangleLines((int)state.m_textBox.x, (int)state.m_textBox.y, (int)state.m_textBox.width,
                           (int)state.m_textBox.height, RED);

        DrawText(state.m_serverIp, (int)state.m_textBox.x + 5, (int)state.m_textBox.y + 8, 40, MAROON);
        DrawText(TextFormat("INPUT CHARS: %i/%i", state.m_letterCount, MAX_INPUT_CHARS), 315, 250, 20, DARKGRAY);

        if (state.m_letterCount < MAX_INPUT_CHARS) {
            if (((state.m_framesCounter / 20) % 2) == 0)
                DrawText("_", (int)state.m_textBox.x + 8 + MeasureText(state.m_serverIp, 40), (int)state.m_textBox.y + 12, 40,
                         MAROON);
        } else
            DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
    } break;

    case GAMEPLAY: {
        state.m_acc += GetFrameTime();

        while (state.m_acc >= state.m_tickDt) {
            // Handle received messages
            HandleReceivedMessages();

            // Send connect request if not connected yet
            if (!state.m_connected && !state.m_disconnected) {
                SendConnectRequest();
            }

            if (state.m_connected && !state.m_disconnected) {
                if (UpdateGameplay() < 0)
                    break;
            }

            state.m_acc -= state.m_tickDt;
        }

        DrawGameplay();
    } break;

    default:
        break;
    }
}

void InitClient(char *serverIp) {
    client = net->NewClient();

    if (!client->Connect(serverIp, PORT)) {
        TraceLog(LOG_WARNING, "Failed to connect to server at %s:%d", serverIp, PORT);
        state.m_disconnected = true;
        state.m_serverCloseCode = -1;
        return;
    }

    TraceLog(LOG_INFO, "Connecting to server at %s:%d", serverIp, PORT);
    state.m_clientInitialized = true;
}

void Fire(void) {
    Missile missile = {.pos = {0}, .rect = {0}, .currentFrame = 0, .framesSpeed = 8, .framesCounter = 0};

    missile.pos.x = state.m_localClientState.x;
    missile.pos.y = state.m_localClientState.y;
    missile.rect.x = state.m_missileAnimationRectangles[missile.currentFrame].x;
    missile.rect.y = state.m_missileAnimationRectangles[missile.currentFrame].y;
    missile.rect.height = state.m_missileAnimationRectangles[missile.currentFrame].height;
    missile.rect.width = state.m_missileAnimationRectangles[missile.currentFrame].width;
    state.m_missiles.push_back(missile);
}

void DrawMissiles(void) {
    state.m_missiles.erase(std::remove_if(state.m_missiles.begin(), state.m_missiles.end(),
                                          [](Missile missile) {
                                              if (missile.pos.x > GAME_WIDTH) {
                                                  TraceLog(LOG_INFO, "Missile out of map... Deleting missile...");
                                                  return true;
                                              }
                                              return false;
                                          }),
                           state.m_missiles.end());

    for (Missile &missile : state.m_missiles) {
        missile.framesCounter++;

        if (missile.framesCounter >= (100 / missile.framesSpeed)) {
            missile.framesCounter = 0;
            missile.currentFrame++;
            if (missile.currentFrame > 4)
                missile.currentFrame = 0;
            missile.rect.x = state.m_missileAnimationRectangles[missile.currentFrame].x;
            missile.rect.y = state.m_missileAnimationRectangles[missile.currentFrame].y;
            missile.rect.height = state.m_missileAnimationRectangles[missile.currentFrame].height;
            missile.rect.width = state.m_missileAnimationRectangles[missile.currentFrame].width;
        }

        missile.pos.x += 5;

        float frameWidth = missile.rect.width;
        float frameHeight = missile.rect.height;
        Rectangle sourceRec = {missile.rect.x, missile.rect.y, frameWidth, frameHeight};
        Rectangle rec = {(float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f};
        Rectangle destRec = {(float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f};
        Vector2 origin = {0.0f, 0.0f};

        DrawTexturePro(state.m_player, sourceRec, destRec, origin, 0.0f, WHITE);
    }

    for (int i = 0; i < MAX_CLIENTS - 1; i++) {
        if (state.m_clients[i]) {
            for (int j = 0; j < state.m_clients[i]->missile_count; j++) {
                Missile missile = state.m_clients[i]->missiles[j];
                float frameWidth = missile.rect.width;
                float frameHeight = missile.rect.height;
                Rectangle sourceRec = {missile.rect.x, missile.rect.y, frameWidth, frameHeight};
                Rectangle rec = {(float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f};
                Rectangle destRec = {(float)missile.pos.x, (float)missile.pos.y, frameWidth * 2.0f, frameHeight * 2.0f};
                Vector2 origin = {0.0f, 0.0f};

                DrawTexturePro(state.m_player, sourceRec, destRec, origin, 0.0f, WHITE);
            }
        }
    }
}

void frame() { UpdateAndDraw(); }

void cleanup() {

    UnloadTexture(state.m_player);
    UnloadTexture(state.m_background);
    UnloadTexture(state.m_mob);

    // Send disconnect message if connected
    if (state.m_clientInitialized && state.m_connected && !state.m_disconnected) {
        Roar::NetBuffer buffer;
        buffer.WriteUInt8(MSG_DISCONNECT);
        client->Send(buffer);
    }

    delete client;
    Roar::PluginSystem::Shutdown();
}

int main(int argc, char *argv) {
    Roar::AppRun(Roar::AppData{.name = "r-type_client",
                               .width = WIDTH,
                               .height = HEIGHT,
                               .headless = false,
                               .init = init,
                               .frame = frame,
                               .cleanup = cleanup});
}
