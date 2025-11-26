#pragma once

#include "pch.h"

// A simple structure to represent connected clients
typedef struct
{
	// Underlying nbnet connection handle, used to send messages to that particular client
	NBN_ConnectionHandle client_handle;

	// Client state
	ClientState state;
} Client;

class Server
{
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
	void HandleChangeColorMessage(ChangeColorMessage* msg, Client* sender);
	void HandleReceivedMessage(void);
	int HandleGameServerEvent(int ev);
	int BroadcastGameState(void);
private:

	unsigned int m_clientCount = 0;
	std::array<Client*, MAX_CLIENTS> m_clients;

	float tick_dt;

	// Spawn positions
	std::vector<Vector2> m_spawns;
};
