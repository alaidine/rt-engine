#pragma once

#if defined(_WIN32) || defined(_WIN64)

#define WIN32_LEAN_AND_MEAN
#define NOGDI          // Exclude GDI definitions to prevent Rectangle conflicts
#define NOUSER         // Exclude USER definitions to prevent function name conflicts

#endif

#include <stdio.h>
#include <signal.h>

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <windows.h>
#include <synchapi.h> 
#else
#include <time.h>
#endif

// Undefine any remaining conflicts just to be safe
#if defined(_WIN32) || defined(_WIN64)

#ifdef Rectangle
#undef Rectangle
#endif
#ifdef CloseWindow
#undef CloseWindow
#endif
#ifdef ShowCursor
#undef ShowCursor
#endif
#ifdef LoadImage
#undef LoadImage
#endif
#ifdef DrawText
#undef DrawText
#endif
#ifdef DrawTextEx
#undef DrawTextEx
#endif
#ifdef PlaySound
#undef PlaySound
#endif

#endif


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
