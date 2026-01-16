#include "framework.h"
#include "RoarEngine.h"
#include "r-type.h"

// A simple structure to represent connected clients
struct ConnectedClient {
    uint32_t client_id;
    sockaddr_in address;
    ClientState state;
    unsigned int last_heard_tick; // For timeout detection
};

struct {
    std::unique_ptr<Roar::NetServer> m_netServer;
    std::unordered_map<uint32_t, ConnectedClient> m_clients;
    uint32_t m_nextClientId = 1;
    unsigned int m_currentTick = 0;
    float tick_dt;

    // Spawn positions
    std::vector<Vector2> m_spawns;
} state;

Roar::NetlibNetwork *net = nullptr;
Roar::NetServer *server = nullptr;

static void init() {
    Roar::PluginSystem::AddPlugin("NetworkPlugin");
    Roar::PluginSystem::Startup();

    net = Roar::GetRegistry()->GetSystem<Roar::NetlibNetwork>("NetlibNetwork");
    server = static_cast<Roar::NetServer*>(net->NewServer(PORT));

    if (!server->Start()) {
        ROAR_ERROR("Failed to start server on port {}", PORT);
    }

    state.m_spawns = {{50, 50}, {GAME_WIDTH - 100, 50}, {50, GAME_HEIGHT - 100}, {GAME_WIDTH - 100, GAME_HEIGHT - 100}};
    state.tick_dt = 1.0f / TICK_RATE;
}

static bool AddressEquals(const sockaddr_in &a, const sockaddr_in &b) {
    return a.sin_addr.s_addr == b.sin_addr.s_addr && a.sin_port == b.sin_port;
}

static ConnectedClient *FindClientByAddress(const sockaddr_in &addr) {
    for (auto &[id, client] : state.m_clients) {
        if (AddressEquals(client.address, addr))
            return &client;
    }
    return nullptr;
}

static uint32_t GetClientIdByAddress(const sockaddr_in &addr) {
    ConnectedClient *client = FindClientByAddress(addr);
    return client ? client->client_id : 0;
}

static void HandleConnectRequest(const sockaddr_in &from) {
    TraceLog(LOG_INFO, "New connection request");

    // Check if already connected
    if (FindClientByAddress(from) != nullptr) {
        TraceLog(LOG_INFO, "Client already connected, ignoring");
        return;
    }

    // Check if server is full
    if (state.m_clients.size() >= MAX_CLIENTS) {
        TraceLog(LOG_INFO, "Server full, rejecting connection");

        Roar::NetBuffer rejectBuffer;
        rejectBuffer.WriteUInt8(MSG_CONNECT_REJECT);
        rejectBuffer.WriteInt32(SERVER_FULL_CODE);
        state.m_netServer->SendTo(rejectBuffer, from);
        return;
    }

    // Create new client
    uint32_t client_id = state.m_nextClientId++;
    Vector2 spawn = state.m_spawns[client_id % MAX_CLIENTS];

    ConnectedClient newClient;
    newClient.client_id = client_id;
    newClient.address = from;
    newClient.state.client_id = client_id;
    newClient.state.x = (int)spawn.x;
    newClient.state.y = (int)spawn.y;
    newClient.state.missile_count = 0;
    memset(newClient.state.missiles, 0, sizeof(newClient.state.missiles));
    newClient.last_heard_tick = state.m_currentTick;

    state.m_clients[client_id] = newClient;

    // Send accept message
    Roar::NetBuffer acceptBuffer;
    acceptBuffer.WriteUInt8(MSG_CONNECT_ACCEPT);

    ConnectAcceptData acceptData;
    acceptData.client_id = client_id;
    acceptData.spawn_x = (int)spawn.x;
    acceptData.spawn_y = (int)spawn.y;
    SerializeConnectAcceptData(acceptBuffer, acceptData);

    server->SendTo(acceptBuffer, from);

    TraceLog(LOG_INFO, "Connection accepted (ID: %d)", client_id);
}

static void HandleDisconnect(uint32_t client_id) {
    auto it = state.m_clients.find(client_id);
    if (it != state.m_clients.end()) {
        TraceLog(LOG_INFO, "Client disconnected (ID: %d)", client_id);
        state.m_clients.erase(it);
    }
}

static void HandleUpdateStateMessage(Roar::NetBuffer &buffer, uint32_t client_id) {
    auto it = state.m_clients.find(client_id);
    if (it == state.m_clients.end())
        return;

    UpdateStateMessage msg = DeserializeUpdateStateMessage(buffer);

    // Update client state
    it->second.state.x = msg.x;
    it->second.state.y = msg.y;
    it->second.state.missile_count = msg.missile_count;
    memset(it->second.state.missiles, 0, sizeof(it->second.state.missiles));
    memcpy(it->second.state.missiles, msg.missiles, msg.missile_count * sizeof(Missile));
    it->second.last_heard_tick = state.m_currentTick;
}

static void HandleReceivedMessage(Roar::NetBuffer &buffer, const sockaddr_in &from) {
    if (!buffer.CanRead(1))
        return;

    uint8_t msgType = buffer.ReadUInt8();

    switch (msgType) {
    case MSG_CONNECT_REQUEST:
        HandleConnectRequest(from);
        break;

    case MSG_DISCONNECT: {
        uint32_t client_id = GetClientIdByAddress(from);
        if (client_id != 0)
            HandleDisconnect(client_id);
        break;
    }

    case MSG_UPDATE_STATE: {
        uint32_t client_id = GetClientIdByAddress(from);
        if (client_id != 0) {
            HandleUpdateStateMessage(buffer, client_id);
        }
        break;
    }

    case MSG_HEARTBEAT: {
        uint32_t client_id = GetClientIdByAddress(from);
        if (client_id != 0) {
            auto it = state.m_clients.find(client_id);
            if (it != state.m_clients.end())
                it->second.last_heard_tick = state.m_currentTick;
        }
        break;
    }
    }
}

static void CheckClientTimeouts(void) {
    std::vector<uint32_t> toRemove;

    for (auto &[id, client] : state.m_clients) {
        if (state.m_currentTick - client.last_heard_tick > CLIENT_TIMEOUT_TICKS) {
            TraceLog(LOG_INFO, "Client timed out (ID: %d)", id);
            toRemove.push_back(id);
        }
    }

    for (uint32_t id : toRemove) {
        state.m_clients.erase(id);
    }
}

static int BroadcastGameState(void) {
    if (state.m_clients.empty())
        return 0;

    // Build game state message
    GameStateMessage gameState;
    gameState.client_count = 0;

    for (auto &[id, client] : state.m_clients) {
        if (gameState.client_count < MAX_CLIENTS) {
            gameState.client_states[gameState.client_count] = client.state;
            gameState.client_count++;
        }
    }

    // Send to all clients
    for (auto &[id, client] : state.m_clients) {
        Roar::NetBuffer buffer;
        buffer.WriteUInt8(MSG_GAME_STATE);
        SerializeGameStateMessage(buffer, gameState);
        server->SendTo(buffer, client.address);
    }

    return 0;
}

static void frame() {
    Roar::NetBuffer recvBuffer;
    sockaddr_in from;

    // Receive and process messages
    while (server->Receive(recvBuffer, from) > 0) {
        HandleReceivedMessage(recvBuffer, from);
        recvBuffer.Clear();
    }

    // Check for client timeouts
    CheckClientTimeouts();

    // Broadcast game state
    if (BroadcastGameState() < 0) {
        TraceLog(LOG_ERROR, "Error broadcasting game states. Exit");
        return;
    }

    state.m_currentTick++;

    // Cap simulation rate
#if defined(_WIN32) || defined(_WIN64)
    Sleep((DWORD)(state.tick_dt * 1000));
#else
    long nanos = (long)(state.tick_dt * 1e9);
    struct timespec t = {.tv_sec = nanos / 999999999, .tv_nsec = nanos % 999999999};
    nanosleep(&t, &t);
#endif
}

static void cleanup() {
    delete server;
    Roar::PluginSystem::Shutdown();
}

int main(int argc, char *argv) {
    (void)argc, (void)argv;
    Roar::AppRun(Roar::AppData{.name = "R-Type (Server)",
                               .width = 1280,
                               .height = 720,
                               .headless = true,
                               .init = init,
                               .frame = frame,
                               .cleanup = cleanup});
}
