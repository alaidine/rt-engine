#pragma once

#include <vector>
#include <array>
#include <memory>
#include <sol/sol.hpp>

#include "shared.h"

// A simple structure to represent connected clients
typedef struct
{
	// Underlying nbnet connection handle, used to send messages to that particular client
	NBN_ConnectionHandle client_handle;

	// Client state
	ClientState state;
} Client;

// Mob structure for server-side management
typedef struct
{
	uint32_t mob_id;
	float x;
	float y;
	bool active;
} Mob;

class Server
{
private:
	unsigned int m_clientCount = 0;
	std::array<Client*, MAX_CLIENTS> m_clients;

	// Mob management
	std::array<Mob, MAX_MOBS> m_mobs;
	unsigned int m_mobCount = 0;
	uint32_t m_nextMobId = 0;
	unsigned int m_mobSpawnTimer = 0;

	float tick_dt;

	// Spawn positions
	std::vector<Vector2> m_spawns;
public:
	Server();
	~Server();

	bool running = true;

	void Init(int argc, char **argv);
	void Run(void);

	void AcceptConnection(unsigned int x, unsigned int y, NBN_ConnectionHandle conn);
	int HandleNewConnection(void);
	Client* FindClientById(uint32_t client_id);
	void DestroyClient(Client* client);
	void HandleClientDisconnection();
	void HandleUpdateStateMessage(UpdateStateMessage* msg, Client* sender);
	void HandleReceivedMessage(void);
	int HandleGameServerEvent(int ev);
	int BroadcastGameState(void);

	// Mob management
	void SpawnMob(void);
	void UpdateMobs(void);
	void CheckMissileCollisions(void);
};
