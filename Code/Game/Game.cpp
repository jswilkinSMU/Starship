#include "Game/Game.h"
#include "Game/GameCommon.h"
#include "Game/PlayerShip.h"
#include "Game/Bullet.h"
#include "Game/Asteroid.h"
#include "Game/Wasp.h"
#include "Game/App.h"
#include "Game/Beetle.h"
#include "Game/BossBeetle.h"
#include "Game/Debris.h"

#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/MathUtils.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.h"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/AABB2.h"

WaveSpawnInfo m_waves[NUM_WAVES] = 
{
	{ 0, 0, 0, 1 },
	{ 10, 2, 0, 0 },
	{ 2, 3, 1, 0 },
	{ 1, 4, 2, 0 },
	{ 0, 5, 3, 0 }
};

Game::Game(App* owner)
	: m_app(owner)
{
}

Game::~Game()
{
}

void Game::StartUp()
{
	SubscribeEventCallbackFunction("GetTimeScale", Event_GetTimeScale);
	SubscribeEventCallbackFunction("SetTimeScale", Event_SetTimeScale);
	// ---------------------------------------------------------------------------------------------------------------------------
	g_theDevConsole->AddLine(Rgba8(102, 153, 204), "Welcome to Starship!");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "------------------------------------------------------------------------");
	g_theDevConsole->AddLine(Rgba8(102, 153, 204), "Gameplay key presses:");
	g_theDevConsole->AddLine(Rgba8::SAPPHIRE, "E to accelerate");
	g_theDevConsole->AddLine(Rgba8::SAPPHIRE, "S and F to rotate");
	g_theDevConsole->AddLine(Rgba8::SAPPHIRE, "N to respawn");
	g_theDevConsole->AddLine(Rgba8::SAPPHIRE, "Spacebar to shoot");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "------------------------------------------------------------------------");
	g_theDevConsole->AddLine(Rgba8(102, 153, 204), "Debug key presses:");
	g_theDevConsole->AddLine(Rgba8::YELLOW, "F1 to debug draw");
	g_theDevConsole->AddLine(Rgba8::YELLOW, "F8 to reset the game");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "------------------------------------------------------------------------");
	g_theDevConsole->AddLine(Rgba8(102, 153, 204), "General key controls:");
	g_theDevConsole->AddLine(Rgba8::ORANGE, "P to pause");
	g_theDevConsole->AddLine(Rgba8::ORANGE, "Hold T to slow down");
	g_theDevConsole->AddLine(Rgba8::ORANGE, "O to single step frames");
	g_theDevConsole->AddLine(Rgba8::ORANGE, "ESC to return to attract mode and exit the game.");
	g_theDevConsole->AddLine(Rgba8::ORANGE, "Spacebar to exit Attract mode.");
	g_theDevConsole->AddLine(Rgba8::SEAWEED, "------------------------------------------------------------------------");
	g_theDevConsole->AddLine(Rgba8::CYAN, "Console command: SetTimeScale scale=value to change timescale through console.");
	// ---------------------------------------------------------------------------------------------------------------------------
	Vec2 miniWorldCenter(MINIWORLD_CENTER_X, MINIWORLD_CENTER_Y);
	m_playerShip = new PlayerShip(this, miniWorldCenter);

	SpawnWaves(0);
	InitializeStars();
}

void Game::Update()
{
	double deltaSeconds = g_theApp->m_gameClock.GetDeltaSeconds();

	AdjustForPauseAndTimeDistortion(static_cast<float>(deltaSeconds));
	UpdateEntities(static_cast<float>(deltaSeconds));
	KeyInputPresses();

	PlayershipvsAsteroids();
	PlayershipvsBeetles();
	PlayershipvsBossBeetles();
	PlayershipvsWasps();

	BulletsvsAsteroids();
	BulletsvsBeetles();
	BulletsvsBossBeetles();
	BulletsvsWasps();

	DeleteGarbageEntities();
	WaveCheck();
	UpdateCameras(static_cast<float>(deltaSeconds));
	GameOver(static_cast<float>(deltaSeconds));
}

void Game::Render() const
{
	if (m_isAttractMode == true)
	{
		g_theRenderer->BeginCamera(m_screenCamera);
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
		RenderAttractMode();
		g_theRenderer->EndCamera(m_screenCamera);
	}
	if (m_isAttractMode == false)
	{
		g_theRenderer->BeginCamera(m_miniWorldCamera);
		RenderEntities();
		RenderStars();
		if (m_isDebugDrawEnabled == true) 
		{
			DebugDrawRender();
		}
		g_theRenderer->EndCamera(m_miniWorldCamera);
		g_theRenderer->BeginCamera(m_screenCamera);
		RenderMiniMap();
		RenderLives();
		g_theRenderer->EndCamera(m_screenCamera);
	}
}

void Game::Shutdown()
{
}

void Game::GameOver(float deltaSeconds)
{
	if (m_gameOver > 0.0f && m_playerLives <= 0) 
	{
		m_gameOver -= deltaSeconds;
		if (m_gameOver <= 0.0f) 
		{
			g_theApp->Shutdown();
			g_theApp->Startup();
			return;
		}
	}
}

void Game::KeyInputPresses()
{
	// Debugging
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1)) //F1 key toggle
	{
		m_isDebugDrawEnabled = !m_isDebugDrawEnabled;
	}

	// Attract Mode
	if (g_theInput->WasKeyJustPressed(' '))
	{
		m_isAttractMode = false;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC))
	{
		m_isAttractMode = true;
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/TestSound.mp3");
		g_theAudio->StartSound(testSound);
	}

	// Playership Respawn
	if (m_playerShip->IsDead())
	{
		if (g_theInput->WasKeyJustPressed('N') && m_playerLives > 0)
		{
			m_playerShip->Respawn();
			SoundID respawnSound = g_theAudio->CreateOrGetSound("Data/Audio/RespawnShip.mp3");
			g_theAudio->StartSound(respawnSound);
			return;
		}
		XboxController const& controller = g_theInput->GetController(0);
		if (controller.WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_START) && m_playerLives > 0)
		{
			m_playerShip->Respawn();
			SoundID respawnSound = g_theAudio->CreateOrGetSound("Data/Audio/RespawnShip.mp3");
			g_theAudio->StartSound(respawnSound);
			return;
		}
	}
}

void Game::AdjustForPauseAndTimeDistortion(float deltaSeconds) {

	UNUSED(deltaSeconds);

	if (g_theInput->IsKeyDown('T'))
	{
		g_theApp->m_gameClock.SetTimeScale(0.1);
	}
	else
	{
		g_theApp->m_gameClock.SetTimeScale(1.0);
	}

	if (g_theInput->WasKeyJustPressed('P'))
	{
		g_theApp->m_gameClock.TogglePause();
	}

	if (g_theInput->WasKeyJustPressed('O'))
	{
		g_theApp->m_gameClock.StepSingleFrame();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) && m_isAttractMode)
	{
		FireEvent("Quit");
	}
}

void Game::UpdateCameras(float deltaSeconds)
{
	m_worldCamera.SetOrthoView(Vec2(0.0f, 0.0f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	AABB2 miniCameraBounds(0.0f, 0.0f, 200.0f, 100.0f);
	AABB2 miniMapCameraBounds(0.0f, 0.0f, 400.f, 400.f);

	if (m_playerShip)
	{
		miniCameraBounds.SetCenter(m_playerShip->GetPosition());
	}
	else
	{
		miniCameraBounds.SetCenter(Vec2(WORLD_CENTER_X, WORLD_CENTER_Y));
	}

	m_miniWorldCamera.SetOrthoView(miniCameraBounds.m_mins, miniCameraBounds.m_maxs);
	m_radarWorldBounds = AABB2(0.f, 0.f, 400.f, 400.f);
	if (m_playerShip)
	{
		m_radarWorldBounds.SetCenter(m_playerShip->GetPosition());
	}
	else
	{
		m_radarWorldBounds.SetCenter(Vec2(WORLD_CENTER_X, WORLD_CENTER_Y));
	}

	float screenShakeAmount = m_screenShakePercent * MAX_SCREEN_SHAKE;
	float screenShakeX = g_rng->RollRandomFloatInRange(-screenShakeAmount, screenShakeAmount);
	float screenShakeY = g_rng->RollRandomFloatInRange(-screenShakeAmount, screenShakeAmount);

	m_miniWorldCamera.Translate2D(Vec2(screenShakeX, screenShakeY));

	m_screenShakePercent -= SCREEN_SHAKE_REDUCTION_PER_SECOND * deltaSeconds;
	m_screenShakePercent = GetClampedZeroToOne(m_screenShakePercent);
}

void Game::AddScreenShake(float amountofShake)
{
	m_screenShakePercent += amountofShake;
}

void Game::DeleteGarbageEntities()
{
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (asteroid && asteroid->IsGarbage())
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet*& bullet = m_bullets[bulletIndex];
		if (bullet && bullet->IsGarbage())
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle*& beetle = m_beetle[beetleIndex];
		if (beetle && beetle->IsGarbage())
		{
			delete beetle;
			beetle = nullptr;
		}
	}

	for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BOSSBEETLES; ++bossBeetleIndex)
	{
		BossBeetle*& bossBeetle = m_bossBeetle[bossBeetleIndex];
		if (bossBeetle && bossBeetle->IsGarbage())
		{
			delete bossBeetle;
			bossBeetle = nullptr;
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		Wasp*& wasp = m_wasp[waspIndex];
		if (wasp && wasp->IsGarbage())
		{
			delete wasp;
			wasp = nullptr;
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		Debris*& debris = m_debris[debrisIndex];
		if (debris && debris->IsGarbage())
		{
			delete debris;
			debris = nullptr;
		}
	}
}

void Game::DebugDrawRender() const
{
	if (!m_isDebugDrawEnabled)
		return;

	if (m_playerShip)
	{
		m_playerShip->DebugRender();
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		Asteroid* asteroid = m_asteroids[asteroidIndex];
		if (asteroid != nullptr)
		{
			asteroid->DebugRender();
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle* beetle = m_beetle[beetleIndex];
		if (beetle != nullptr)
		{
			beetle->DebugRender();
		}
	}

	for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BOSSBEETLES; ++bossBeetleIndex)
	{
		BossBeetle* bossBeetle = m_bossBeetle[bossBeetleIndex];
		if (bossBeetle != nullptr)
		{
			bossBeetle->DebugRender();
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		Wasp* wasp = m_wasp[waspIndex];
		if (wasp != nullptr)
		{
			wasp->DebugRender();
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (bullet != nullptr)
		{
			bullet->DebugRender();
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		Debris* debris = m_debris[debrisIndex];
		if (debris != nullptr)
		{
			debris->DebugRender();
		}
	}
}

Vec2 Game::GetRandomOffScreenPosition(float cosmeticRadius)
{
	
	int side = g_rng->RollRandomIntLessThan(4);
	// North
	if (side == 0)
	{
		float y = -cosmeticRadius;
		float x = g_rng->RollRandomFloatInRange(cosmeticRadius, WORLD_SIZE_X - cosmeticRadius);
		return Vec2(x, y);
	}
	// East
	else if (side == 1)
	{
		float x = WORLD_SIZE_X + cosmeticRadius;
		float y = g_rng->RollRandomFloatInRange(cosmeticRadius, WORLD_SIZE_Y - cosmeticRadius);
		return Vec2(x, y);
	}
	// South
	else if (side == 2)
	{
		float y = WORLD_SIZE_Y + cosmeticRadius;
		float x = g_rng->RollRandomFloatInRange(cosmeticRadius, WORLD_SIZE_X - cosmeticRadius);
		return Vec2(x, y);
	}
	// West
	else if (side == 3)
	{
		float x = -cosmeticRadius;
		float y = g_rng->RollRandomFloatInRange(cosmeticRadius, WORLD_SIZE_Y - cosmeticRadius);
		return Vec2(x, y);
	}

	return Vec2(0, 0);
}

Vec2 Game::MiniMapScale(Vec2 const& worldPosition)
{
	return Vec2(RangeMapClamped(worldPosition.x, 0.0f, WORLD_SIZE_X, 0.0f, MINIMAP_WIDTH),RangeMapClamped(worldPosition.y, 0.0f, WORLD_SIZE_Y, 0.0f, MINIMAP_HEIGHT));
}

bool Game::Event_SetTimeScale(EventArgs& args)
{
	std::string scaleAmount = args.GetValue("scale", "");
	if (scaleAmount.empty())
	{
		g_theDevConsole->AddLine(Rgba8::RED, "Missing argument! Correct argument: SetTimeScale scale=value");
		return false;
	}

	double scale = std::stod(scaleAmount);
	if (scale < 0.0f)
	{
		g_theDevConsole->AddLine(Rgba8::RED, "Invalid! Argument must be zero or greater.");
		return false;
	}

	Clock::GetSystemClock().SetTimeScale(scale);
	g_theDevConsole->AddLine(Rgba8::LIMEGREEN, Stringf("Time scale set to: %.2f", scale));
	return true;
}

bool Game::Event_GetTimeScale(EventArgs& args)
{
	UNUSED(args);

	double currentTimeScale = Clock::GetSystemClock().GetTimeScale();
	g_theDevConsole->AddLine(Rgba8::LIMEGREEN, Stringf("Current Time Scale: %.2f", currentTimeScale));

	return true;
}


void Game::CheckBulletVsEnemyList(Bullet& bullet, Entity** enemyList, int maxListSize)
{
	for (int enemyListIndex = 0; enemyListIndex < maxListSize; ++enemyListIndex)
	{
		Entity* enemyEntity = enemyList[enemyListIndex];
		if (enemyEntity != nullptr && DoDiscsOverlap(bullet.GetPosition(), bullet.GetPhysicsRadius(), enemyEntity->GetPosition(), enemyEntity->GetPhysicsRadius()))
		{
			bullet.BulletDie();

			if (Asteroid* asteroid = dynamic_cast<Asteroid*>(enemyEntity))
			{
				asteroid->TakeHit();
			}
			if (Beetle* beetle = dynamic_cast<Beetle*>(enemyEntity))
			{
				beetle->TakeHit();
			}
			if (BossBeetle* bossBeetle = dynamic_cast<BossBeetle*>(enemyEntity))
			{
				bossBeetle->TakeHit();
			}
			if (Wasp* wasp = dynamic_cast<Wasp*>(enemyEntity))
			{
				wasp->TakeHit();
			}
		}
	}
}

void Game::BulletsvsAsteroids()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (m_bullets[bulletIndex] != nullptr)
		{
			for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
			{
				if (m_asteroids[asteroidIndex] != nullptr)
				{
					if (DoDiscsOverlap(bullet->GetPosition(), bullet->GetPhysicsRadius(), m_asteroids[asteroidIndex]->GetPosition(), m_asteroids[asteroidIndex]->GetPhysicsRadius()))
					{
						bullet->BulletDie();
						m_asteroids[asteroidIndex]->TakeHit();
					}
				}
			}
		}
	}
}

void Game::BulletsvsBeetles()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (m_bullets[bulletIndex] != nullptr)
		{
			for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
			{
				if (m_beetle[beetleIndex] != nullptr)
				{
					if (DoDiscsOverlap(bullet->GetPosition(), bullet->GetPhysicsRadius(), m_beetle[beetleIndex]->GetPosition(), m_beetle[beetleIndex]->GetPhysicsRadius()))
					{
						bullet->BulletDie();
						m_beetle[beetleIndex]->TakeHit();
					}
				}
			}
		}
	}
}

void Game::BulletsvsBossBeetles()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (m_bullets[bulletIndex] != nullptr)
		{
			for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BEETLES; ++bossBeetleIndex)
			{
				if (m_bossBeetle[bossBeetleIndex] != nullptr)
				{
					if (DoDiscsOverlap(bullet->GetPosition(), bullet->GetPhysicsRadius(), m_bossBeetle[bossBeetleIndex]->GetPosition(), m_bossBeetle[bossBeetleIndex]->GetPhysicsRadius()))
					{
						bullet->BulletDie();
						m_bossBeetle[bossBeetleIndex]->TakeHit();
					}
				}
			}
		}
	}
}

void Game::BulletsvsWasps()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = m_bullets[bulletIndex];
		if (m_bullets[bulletIndex] != nullptr)
		{
			for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
			{
				if (m_wasp[waspIndex] != nullptr)
				{
					if (DoDiscsOverlap(bullet->GetPosition(), bullet->GetPhysicsRadius(), m_wasp[waspIndex]->GetPosition(), m_wasp[waspIndex]->GetPhysicsRadius()))
					{
						bullet->BulletDie();
						m_wasp[waspIndex]->TakeHit();
					}
				}
			}
		}
	}
}

void Game::PlayershipvsAsteroids()
{
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if (m_asteroids[asteroidIndex] != nullptr) 
		{
			if (DoDiscsOverlap(m_playerShip->GetPosition(), m_playerShip->GetPhysicsRadius(),
				m_asteroids[asteroidIndex]->GetPosition(), m_asteroids[asteroidIndex]->GetPhysicsRadius()))
			{
				if (!m_playerShip->IsDead())
				{
					m_playerShip->PlayerShipDie();
					UpdateLives();
				}
			}
		}
	}
}

void Game::PlayershipvsBeetles()
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_beetle[beetleIndex] != nullptr) 
		{
			if (DoDiscsOverlap(m_playerShip->GetPosition(), m_playerShip->GetPhysicsRadius(),
				m_beetle[beetleIndex]->GetPosition(), m_beetle[beetleIndex]->GetPhysicsRadius()))
			{
				if (!m_playerShip->IsDead()) 
				{
					m_playerShip->PlayerShipDie();
					UpdateLives();
				}
			}
		}
	}
}

void Game::PlayershipvsBossBeetles()
{
	for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BEETLES; ++bossBeetleIndex)
	{
		if (m_bossBeetle[bossBeetleIndex] != nullptr)
		{
			if (DoDiscsOverlap(m_playerShip->GetPosition(), m_playerShip->GetPhysicsRadius(),
				m_bossBeetle[bossBeetleIndex]->GetPosition(), m_bossBeetle[bossBeetleIndex]->GetPhysicsRadius()))
			{
				if (!m_playerShip->IsDead())
				{
					m_playerShip->PlayerShipDie();
					UpdateLives();
				}
			}
		}
	}
}

void Game::PlayershipvsWasps()
{
	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_wasp[waspIndex] != nullptr) 
		{
			if (DoDiscsOverlap(m_playerShip->GetPosition(), m_playerShip->GetPhysicsRadius(),
				m_wasp[waspIndex]->GetPosition(), m_wasp[waspIndex]->GetPhysicsRadius()))
			{
				if (!m_playerShip->IsDead()) 
				{
					m_playerShip->PlayerShipDie();
					UpdateLives();
				} 
			}
		}
	}
}

void Game::WaveCheck()
{
	if (m_waveCleared) {
		m_currentWave += 1;
		if (m_currentWave < NUM_WAVES) {
			SpawnWaves(m_currentWave);
			m_waveCleared = false;
		}
		else {
			m_isAttractMode = true;
			g_theApp->Shutdown();
			g_theApp->Startup();
			SoundID victorySound = g_theAudio->CreateOrGetSound("Data/Audio/StarshipVictory.mp3");
			g_theAudio->StartSound(victorySound);
			return;
		}
	}

	if (AllEnemyEntitiesCleared()) {
		m_waveCleared = true;
		SoundID newWaveSound = g_theAudio->CreateOrGetSound("Data/Audio/LevelClear.mp3");
		g_theAudio->StartSound(newWaveSound);
	}
}

void Game::SpawnWaves(int wavesIndex)
{
	const WaveSpawnInfo& waveInfo = m_waves[wavesIndex];

	for (int i = 0; i < waveInfo.numAsteroids; ++i) 
	{
		SpawnRandomAsteroid();
	}
	for (int i = 0; i < waveInfo.numBeetles; ++i) 
	{
		SpawnRandomBeetle(GetRandomOffScreenPosition(BEETLE_COSMETIC_RADIUS), g_rng->RollRandomFloatInRange(0.f, 360.f));
	}
	for (int i = 0; i < waveInfo.numWasps; ++i) 
	{
		SpawnRandomWasp();
	}
	for (int i = 0; i < waveInfo.numBossBeetles; ++i)
	{
		SpawnRandomBossBeetle();
	}
}

bool Game::AllEnemyEntitiesCleared() 
{
	for (int i = 0; i < MAX_ASTEROIDS; ++i) 
	{
		if (m_asteroids[i] != nullptr) 
		{
			return false;
		}
	}
	for (int i = 0; i < MAX_BEETLES; ++i) 
	{
		if (m_beetle[i] != nullptr)
		{
			return false;
		}
	}
	for (int i = 0; i < MAX_WASPS; ++i) 
	{
		if (m_wasp[i] != nullptr)
		{
			return false;
		}
	}
	for (int i = 0; i < MAX_BOSSBEETLES; ++i)
	{
		if (m_bossBeetle[i] != nullptr)
		{
			return false;
		}
	}
	return true;
}


PlayerShip* Game::GetPlayerShip() const
{
	return m_playerShip;
}

Bullet* Game::SpawnBullet(Vec2 const& pos, float forwardDegrees)
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		Bullet*& bullet = m_bullets[bulletIndex];
		if (!bullet)
		{
			bullet = new Bullet(this, pos, forwardDegrees);
			return bullet;
		}
	}
	ERROR_AND_DIE("Cannot spawn a new bullet; all slots are full!");
}

Asteroid* Game::SpawnRandomAsteroid()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition(ASTEROID_COSMETIC_RADIUS);

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (!asteroid)
		{
			asteroid = new Asteroid(this, spawnPosition);
			return asteroid;
		}
	}
	ERROR_AND_DIE("Cannot spawn a new asteroid; all slots are full!");
}

Beetle* Game::SpawnRandomBeetle(Vec2 const& spawnPosition, float forwardDegrees)
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		Beetle*& beetle = m_beetle[beetleIndex];
		if (!beetle)
		{
			beetle = new Beetle(this, spawnPosition, forwardDegrees, m_playerShip);
			return beetle;
		}
	}
	ERROR_AND_DIE("Cannot spawn a new beetle; all slots are full!");
}

BossBeetle* Game::SpawnRandomBossBeetle()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition(BOSSBEETLE_COSMETIC_RADIUS);

	for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BOSSBEETLES; ++bossBeetleIndex)
	{
		BossBeetle*& bossBeetle = m_bossBeetle[bossBeetleIndex];
		if (!bossBeetle)
		{
			bossBeetle = new BossBeetle(this, spawnPosition, m_playerShip);
			return bossBeetle;
		}
	}
	ERROR_AND_DIE("Cannot spawn a new beetle; all slots are full!");
}


Wasp* Game::SpawnRandomWasp()
{
	Vec2 spawnPosition = GetRandomOffScreenPosition(WASP_COSMETIC_RADIUS);

	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		Wasp*& wasp = m_wasp[waspIndex];
		if (!wasp)
		{
			wasp = new Wasp(this, spawnPosition, m_playerShip);
			return wasp;
		}
	}
	ERROR_AND_DIE("Cannot spawn a new wasp; all slots are full!");
}

Debris* Game::SpawnDebrisCluster(int numDebris, Vec2 const& position, Vec2 const& averageVelocity, float maxScatterSpeed, float averageRadius, const Rgba8& color)
{
	for (int i = 0; i < numDebris; ++i)
	{
		for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
		{
			if (!m_debris[debrisIndex]) 
			{
				// Spawn the debris
				m_debris[debrisIndex] = new Debris(this, position, averageVelocity, averageRadius, maxScatterSpeed, color);
				break; 
			}
		}
	}
	return nullptr; 
}

void Game::UpdateEntities(float deltaSeconds)
{
	if (m_playerShip && !m_playerShip->IsDead()) 
	{
		m_playerShip->Update(deltaSeconds);
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_beetle[beetleIndex] != nullptr)
		{
			m_beetle[beetleIndex]->Update(deltaSeconds);
		}
	}

	for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BOSSBEETLES; ++bossBeetleIndex)
	{
		if (m_bossBeetle[bossBeetleIndex] != nullptr)
		{
			m_bossBeetle[bossBeetleIndex]->Update(deltaSeconds);
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_wasp[waspIndex] != nullptr)
		{
			m_wasp[waspIndex]->Update(deltaSeconds);
		}
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if (m_asteroids[asteroidIndex] != nullptr)
		{
			m_asteroids[asteroidIndex]->Update(deltaSeconds);
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		if (m_debris[debrisIndex] != nullptr)
		{
			m_debris[debrisIndex]->Update(deltaSeconds);
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if (m_bullets[bulletIndex] != nullptr)
		{
			m_bullets[bulletIndex]->Update(deltaSeconds);
			//CheckBulletVsEnemyList(*m_bullets[bulletIndex], m_beetle, MAX_BEETLES);
		}
	}
}

void Game::RenderAttractMode() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));
	Vertex_PCU startTriangle[STARTTRIANGLE_VERTS];
	float alpha = RangeMap(sinf(static_cast<float>(GetCurrentTimeSeconds())), 2.f, 3.f, 255.f, 127.f);

	startTriangle[0].m_position = Vec3(2.f, 0.0f, 0.0f);
	startTriangle[1].m_position = Vec3(0.0f, 1.f, 0.0f);
	startTriangle[2].m_position = Vec3(0.0f, -1.f, 0.0f);

	for (int vertIndex = 0; vertIndex < STARTTRIANGLE_VERTS; ++vertIndex)
	{
		startTriangle[vertIndex].m_color = Rgba8(0, 255, 0, static_cast<unsigned char>(alpha));
	}

	TransformVertexArrayXY3D(STARTTRIANGLE_VERTS, startTriangle, 150.f, 0.0f, Vec2(700.f, SCREEN_CENTER_Y));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(STARTTRIANGLE_VERTS, startTriangle);

	Vertex_PCU fakeShip[NUM_SHIP_VERTS];
	PlayerShip::InitializeVerts(&fakeShip[0]);
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, &fakeShip[0], 100.f, 0.0f, Vec2(350.f, SCREEN_CENTER_Y));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, &fakeShip[0]);

	PlayerShip::InitializeVerts(&fakeShip[0]);
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, &fakeShip[0], 100.f, 180.0f, Vec2(1300.f, SCREEN_CENTER_Y));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, &fakeShip[0]);
}

void Game::UpdateLives()
{
	if (m_playerShip->IsDead() && m_playerLives > 0)
	{
		m_playerLives -= 1;
		if (m_playerLives <= 0)
		{
			SoundID gameOverSound = g_theAudio->CreateOrGetSound("Data/Audio/GameOver.mp3");
			g_theAudio->StartSound(gameOverSound);
			GameOver(1.f / 60.f);
		}
	}
}


void Game::RenderLives() const
{
	Vertex_PCU fakeShip[NUM_SHIP_VERTS];
	Vec2 positions[3] = 
	{ 
		Vec2(50.f, 775.f), Vec2(100.f, 775.f), Vec2(150.f, 775.f) 
	};

	for (int i = 0; i < m_playerLives; ++i)
	{
		if (i < 3)
		{
			PlayerShip::InitializeVerts(&fakeShip[0]);
			TransformVertexArrayXY3D(NUM_SHIP_VERTS, &fakeShip[0], 8.f, 90.0f, positions[i]);
			g_theRenderer->BindTexture(nullptr);
			g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, &fakeShip[0]);
		}
		else
		{
			PlayerShip::InitializeVerts(&fakeShip[0]);
			TransformVertexArrayXY3D(NUM_SHIP_VERTS, &fakeShip[0], 8.f, 90.0f, Vec2(50.f + (i * 50.f), 775.f));
			g_theRenderer->BindTexture(nullptr);
			g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, &fakeShip[0]);
		}
	}
}

void Game::RenderMiniMap() const
{
	if (m_playerShip)
	{
		Vec2 worldPos = m_playerShip->GetPosition();
		float screenPosX = RangeMap(worldPos.x, m_radarWorldBounds.m_mins.x, m_radarWorldBounds.m_maxs.x, 50.f, 100.f);
		float screenPosY = RangeMap(worldPos.y, m_radarWorldBounds.m_mins.y, m_radarWorldBounds.m_maxs.y, 50.f, 100.f);
		Vec2 screenPos = Vec2(screenPosX, screenPosY);
		DebugDrawRing(Vec2(screenPosX, screenPosY), 0.5f, 5.f, Rgba8(102, 153, 204));
	}
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_beetle[beetleIndex] != nullptr)
		{
			Vec2 worldPos = m_beetle[beetleIndex]->GetPosition();
			float screenPosX = RangeMap(worldPos.x, m_radarWorldBounds.m_mins.x, m_radarWorldBounds.m_maxs.x, 50.f, 100.f);
			float screenPosY = RangeMap(worldPos.y, m_radarWorldBounds.m_mins.y, m_radarWorldBounds.m_maxs.y, 50.f, 100.f);
			Vec2 screenPos = Vec2(screenPosX, screenPosY);
			DebugDrawRing(Vec2(screenPosX, screenPosY), 0.5f, 10.f, Rgba8(0, 255, 0));
		}
	}

	Vertex_PCU miniMap[MINIMAP_VERTS];

	// Left vertical line
	miniMap[0].m_position = Vec3(0.0f, 0.0f, 0.0f);
	miniMap[1].m_position = Vec3(0.0f, 10.f, 0.0f);
	miniMap[2].m_position = Vec3(1.0f, 0.f, 0.0f);
	miniMap[3].m_position = Vec3(1.0f, 10.0f, 0.0f);
	miniMap[4].m_position = Vec3(0.0f, 10.f, 0.0f);
	miniMap[5].m_position = Vec3(1.0f, 0.f, 0.0f);

	// Top horizontal Line
	miniMap[6].m_position = Vec3(1.0f, 10.0f, 0.0f);
	miniMap[7].m_position = Vec3(1.0f, 9.f, 0.0f);
	miniMap[8].m_position = Vec3(24.0f, 10.f, 0.0f);
	miniMap[9].m_position = Vec3(1.0f, 9.0f, 0.0f);
	miniMap[10].m_position = Vec3(24.0f, 9.f, 0.0f);
	miniMap[11].m_position = Vec3(24.0f, 10.f, 0.0f);

	// Right vertical Line
	miniMap[12].m_position = Vec3(24.0f, 10.0f, 0.0f);
	miniMap[13].m_position = Vec3(25.0f, 10.0f, 0.0f);
	miniMap[14].m_position = Vec3(25.0f, 0.0f, 0.0f);
	miniMap[15].m_position = Vec3(24.0f, 0.0f, 0.0f);
	miniMap[16].m_position = Vec3(25.0f, 0.f, 0.0f);
	miniMap[17].m_position = Vec3(24.0f, 10.f, 0.0f);

	// Bottom Horizontal Line
	miniMap[18].m_position = Vec3(24.0f, 0.0f, 0.0f);
	miniMap[19].m_position = Vec3(1.0f, 0.0f, 0.0f);
	miniMap[20].m_position = Vec3(1.0f, 1.0f, 0.0f);
	miniMap[21].m_position = Vec3(24.0f, 1.0f, 0.0f);
	miniMap[22].m_position = Vec3(1.0f, 1.0f, 0.0f);
	miniMap[23].m_position = Vec3(24.0f, 0.0f, 0.0f);

	for (int vertIndex = 0; vertIndex < MINIMAP_VERTS; ++vertIndex)
	{
		miniMap[vertIndex].m_color = Rgba8(255, 255, 255, 255);
	}

	TransformVertexArrayXY3D(MINIMAP_VERTS, &miniMap[0], 10.f, 0.0f, Vec2(50, 50));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(MINIMAP_VERTS, &miniMap[0]);

}

void Game::RenderEntities() const
{
	if (m_playerShip) 
	{
		m_playerShip->Render();
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_beetle[beetleIndex] != nullptr)
		{
			m_beetle[beetleIndex]->Render();
		}
	}

	for (int bossBeetleIndex = 0; bossBeetleIndex < MAX_BOSSBEETLES; ++bossBeetleIndex)
	{
		if (m_bossBeetle[bossBeetleIndex] != nullptr)
		{
			m_bossBeetle[bossBeetleIndex]->Render();
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_wasp[waspIndex] != nullptr)
		{
			m_wasp[waspIndex]->Render();
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if (m_bullets[bulletIndex] != nullptr) 
		{
			m_bullets[bulletIndex]->Render();
		}
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if (m_asteroids[asteroidIndex] != nullptr)
		{
			m_asteroids[asteroidIndex]->Render();
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		if (m_debris[debrisIndex] != nullptr)
		{
			m_debris[debrisIndex]->Render();
		}
	}
}

void Game::RenderStars() const
{
	for (int i = 0; i < NUM_STARS; ++i)
	{
		Vertex_PCU tempStarVerts[STAR_VERTS];

		for (int vertIndex = 0; vertIndex < STAR_VERTS; ++vertIndex)
		{
			tempStarVerts[vertIndex] = m_starVerts[vertIndex];
		}

		TransformVertexArrayXY3D(STAR_VERTS, tempStarVerts, 0.25f, 0.0f, m_starPosition[i]);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray(STAR_VERTS, &tempStarVerts[0]);
	}
}

void Game::InitializeStars()
{
	for (int i = 0; i < NUM_STARS; ++i)
	{
		float randomX = g_rng->RollRandomFloatInRange(0.0f, 2000.f);
		float randomY = g_rng->RollRandomFloatInRange(0.0f, 1000.f);
		m_starPosition[i] = Vec2(randomX, randomY);
	}
	m_starVerts[0].m_position = Vec3(2.f, 0.0f, 0.0f);
	m_starVerts[1].m_position = Vec3(0.0f, 1.f, 0.0f);
	m_starVerts[2].m_position = Vec3(0.0f, -1.f, 0.0f);

	for (int vertIndex = 0; vertIndex < STAR_VERTS; ++vertIndex)
	{
		m_starVerts[vertIndex].m_color = Rgba8(255, 255, 255, 255);
	}
}
