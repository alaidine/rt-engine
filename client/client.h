#pragma once

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <sol/sol.hpp>

#include "shared.h"

// ECS includes
#include "../ECS/Core.hpp"
#include "../ECS/Prefab.hpp"
#include "../ECS/Builder/Builder.hpp"
#include "../ECS/System/ClientRendererSystem.hpp"

#define TARGET_FPS 100
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

	// ECS Core
	Core m_ecsCore;
	std::shared_ptr<ClientRendererSystem> m_clientRendererSystem;
	Entity m_localPlayerEntity;
	std::unordered_map<uint32_t, Entity> m_clientEntities; // Maps client_id to Entity
	std::unordered_map<uint32_t, std::vector<Missile>> m_remoteMissiles; // Maps client_id to their missiles

	// Mob storage (received from server)
	std::vector<MobState> m_mobs;

	bool m_clientInitialized;
	bool m_connected;               // Connected to the server
	bool m_disconnected;            // Got disconnected from the server
	bool m_spawned;                 // Has spawned
	int m_serverCloseCode;          // The server code used when closing the connection
	uint32_t m_localClientId;       // Local client ID from server
	GameScreen m_currentScreen;
	std::array<int, MAX_CLIENTS> m_updatedIds;
	std::array<Rectangle, 5> m_missileAnimationRectangles;
	std::vector<Missile> m_missiles;
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

	// ECS initialization
	void InitECS(void);
	
public:
	Client();
	~Client();

	void Init(void);
	void Run(void);

	void Fire(void);
	void DrawMissiles(void);
	void DrawMobs(void);

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

	int UpdateGameplay(void);
	void UpdateClient(ClientState state);
	
	void UpdateAndDraw(void);
	void DrawClient(Entity entity);
	void DrawHUD(void);
	void DrawGameplay(void);
	void DrawBackground(void);

	void InitClient(char* serverIp);
};
