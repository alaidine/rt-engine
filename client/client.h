#pragma once

#include <stdio.h>

#include <array>
#include <sol/sol.hpp>
#include <memory>

#include "shared.h"

#define TARGET_FPS 60
#define MAX_INPUT_CHARS 15

class Client
{
private:
	typedef enum
	{
		TITLE = 0,
		IP_ADDRESS,
		GAMEPLAY
	} GameScreen;

	bool m_clientInitialized;
	bool m_connected;               // Connected to the server
	bool m_disconnected;            // Got disconnected from the server
	bool m_spawned;                 // Has spawned
	int m_serverCloseCode;          // The server code used when closing the connection
	ClientState m_localClientState; // The state of the local client
	GameScreen m_currentScreen;
	std::array<ClientState*, MAX_CLIENTS - 1> m_clients;
	std::array<int, MAX_CLIENTS> m_updatedIds;
	unsigned int m_clientCount;
	bool m_colorKeyPressed;
	double m_tickDt; // Tick delta time (in seconds)
	double m_acc;
	char m_serverIp[MAX_INPUT_CHARS + 1]; // NOTE: One extra space required for null terminator char '\0'
	int m_letterCount;
	Rectangle m_textBox;
	int m_framesCounter;
	bool m_displayHUD;
	Texture2D m_player;
	Texture2D m_background;

public:
	Client();
	~Client();

	void Init(void);
	void Run(void);

	void SpawnLocalClient(int x, int y, uint32_t client_id);
	bool ClientExists(uint32_t client_id);
	void CreateClient(ClientState state);
	
	void DestroyClient(uint32_t client_id);
	void DestroyDisconnectedClients(void);

	void HandleConnection(void);
	void HandleDisconnection(void);
	void HandleGameStateMessage(GameStateMessage* msg);
	void HandleReceivedMessage(void);
	void HandleGameClientEvent(int ev);

	int SendPositionUpdate(void);
	int SendColorUpdate(void);

	int Update(void);
	void UpdateClient(ClientState state);
	
	void UpdateAndDraw(void);
	void DrawClient(ClientState* state, bool is_local);
	void DrawHUD(void);
	void Draw(void);
	void DrawBackground(void);

	void InitClient(char* serverIp);
};
