#pragma once
#include "Game/GameCommon.h"

#include "Engine/Renderer/Camera.h"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Vertex_PCU.h"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/AABB2.h"

//--------------------------------------------------------------------
//Forward declarations of files
class App;
class Entity;
class PlayerShip;
class Asteroid;
class Bullet;
class Beetle;
class BossBeetle;
class Wasp;
class Debris;

//--------------------------------------------------------------------
struct WaveSpawnInfo
{
	int numAsteroids;
	int numBeetles;
	int numWasps;
	int numBossBeetles;
};

//--------------------------------------------------------------------
class Game
{
public:
	App* m_app;
	Game(App* owner);
	~Game();
	void StartUp();

	void Update();
	void UpdateLives();
	void UpdateCameras(float deltaSeconds);
	void AddScreenShake(float amountofShake);

	void Render() const;
	void RenderStars() const;
	void RenderAttractMode() const;
	void RenderLives() const;
	void RenderMiniMap() const;

	void Shutdown();
	void GameOver(float deltaSeconds);

	void KeyInputPresses();
	void AdjustForPauseAndTimeDistortion(float deltaSeconds);

	void DeleteGarbageEntities();
	void DebugDrawRender() const;

	Vec2 GetRandomOffScreenPosition(float m_cosmeticRadius);
	Vec2 MiniMapScale(Vec2 const& worldPosition);

	static bool Event_SetTimeScale(EventArgs& args);
	static bool Event_GetTimeScale(EventArgs& args);

public:
	void CheckBulletVsEnemyList(Bullet& bullet, Entity** enemyList, int MaxListSize);
	void BulletsvsAsteroids();
	void BulletsvsBeetles();
	void BulletsvsBossBeetles();
	void BulletsvsWasps();
	void PlayershipvsAsteroids();
	void PlayershipvsBeetles();
	void PlayershipvsBossBeetles();
	void PlayershipvsWasps();

public:
	bool		m_isAttractMode = true;
	PlayerShip* m_playerShip = nullptr;
	Beetle*		m_beetle[MAX_BEETLES] = {};
	BossBeetle* m_bossBeetle[MAX_BOSSBEETLES] = {};
	Wasp*		m_wasp[MAX_WASPS] = {};
	Asteroid*	m_asteroids[MAX_ASTEROIDS] = {}; 
	Bullet*		m_bullets[MAX_BULLETS] = {}; 
	Debris*		m_debris[MAX_DEBRIS] = {};
	
public:
	PlayerShip* GetPlayerShip() const;
	Bullet*		SpawnBullet(Vec2 const& pos, float forwardDegrees);
	Asteroid*	SpawnRandomAsteroid();
	Beetle*		SpawnRandomBeetle(Vec2 const& spawnPosition, float forwardDegrees);
	BossBeetle* SpawnRandomBossBeetle();
	Wasp*		SpawnRandomWasp();
	Debris*		SpawnDebrisCluster(int numDebris, Vec2 const& position, Vec2 const& averageVelocity, float spraySpeed, float averageRadius, const Rgba8& color);

private:
	void UpdateEntities(float deltaSeconds);
	void RenderEntities() const;
	void WaveCheck();
	void SpawnWaves(int wavesIndex);
	bool AllEnemyEntitiesCleared();

	void InitializeStars();
	Vertex_PCU m_starVerts[STAR_VERTS];
	Vec2 m_starPosition[NUM_STARS];

	bool   m_isDebugDrawEnabled = false;

	int	   m_playerLives = 3;
	float  m_gameOver = 3.0f;

	Camera m_worldCamera;
	Camera m_screenCamera;
	Camera m_miniWorldCamera;
	AABB2  m_radarWorldBounds;
	float  m_screenShakePercent = 0.0f;

	int	 m_currentWave = 0;
	bool m_waveCleared = false;

private:
	
};