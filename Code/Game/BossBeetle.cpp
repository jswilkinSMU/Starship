#include "Beetle.h"
#include "BossBeetle.h"
#include "Game.h"

#include <Engine/Core/VertexUtils.h>
#include "Engine/Renderer/Renderer.h"
#include <Engine/Math/MathUtils.h>
#include <Engine/Audio/AudioSystem.hpp>


BossBeetle::BossBeetle(Game* owner, Vec2 const& startPos, Entity* PlayerShip)
	: Entity(owner, startPos), m_playerShip(PlayerShip)
{
	m_health = 50;
	m_physicsRadius = BOSSBEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BOSSBEETLE_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

BossBeetle::~BossBeetle()
{
}

void BossBeetle::Update(float deltaSeconds)
{
	BossBeetleMovement();
	m_position += (m_velocity * deltaSeconds);

	m_spawnTime += deltaSeconds;
	if (m_spawnTime >= m_spawnInterval) 
	{
		SpawnBeetles();
		m_spawnTime = 0.0f;
	}

	if (m_isHit) 
	{
		m_hitFlash -= deltaSeconds;
		if (m_hitFlash <= 0.0f) 
		{
			m_isHit = false; 
			m_hitFlash = 0.0f;
		}
	}
}

void BossBeetle::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_BOSSBEETLE_VERTS];

	Rgba8 bodyColor;

	if (m_isHit == true) 
	{
		bodyColor = Rgba8(255, 0, 0);
	}
	else 
	{
		bodyColor = Rgba8(0, 255, 0);
	}

	for (int vertIndex = 0; vertIndex < NUM_BOSSBEETLE_VERTS; ++vertIndex) 
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
		if (vertIndex < 30 || vertIndex > 35) 
		{ 
			tempWorldVerts[vertIndex].m_color = bodyColor;
		}
		else 
		{
			tempWorldVerts[vertIndex].m_color = Rgba8(255, 0, 0, 255);
		}
	}
	TransformVertexArrayXY3D(NUM_BOSSBEETLE_VERTS, tempWorldVerts, 7.5f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_BOSSBEETLE_VERTS, tempWorldVerts);
}

void BossBeetle::BossBeetleDie()
{
	m_isDead = true;
	m_isGarbage = true;
	SoundID enemyDefeatSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDefeat.mp3");
	g_theAudio->StartSound(enemyDefeatSound);
	m_game->SpawnDebrisCluster(7, m_position, m_velocity, 90.0f, 1.f, Rgba8(0, 255, 0));
}


void BossBeetle::InitializeLocalVerts()
{
	// Boss Beetle top
	m_localVerts[0].m_position = Vec3(0.0f, 2.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(3.0f, 1.f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[0].m_color = Rgba8(0, 255, 0);
	m_localVerts[1].m_color = Rgba8(0, 255, 0);
	m_localVerts[2].m_color = Rgba8(0, 255, 0);

	// Boss Beetle mid
	m_localVerts[3].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[4].m_position = Vec3(3.0f, 1.f, 0.0f);
	m_localVerts[5].m_position = Vec3(3.0f, -1.0f, 0.0f);
	m_localVerts[3].m_color = Rgba8(0, 255, 0);
	m_localVerts[4].m_color = Rgba8(0, 255, 0);
	m_localVerts[5].m_color = Rgba8(0, 255, 0);

	// Boss Beetle bottom
	m_localVerts[6].m_position = Vec3(3.0f, -1.0f, 0.0f);
	m_localVerts[7].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[8].m_position = Vec3(0.0f, -2.0f, 0.0f);
	m_localVerts[6].m_color = Rgba8(0, 255, 0);
	m_localVerts[7].m_color = Rgba8(0, 255, 0);
	m_localVerts[8].m_color = Rgba8(0, 255, 0);

	// Reversed top
	m_localVerts[9].m_position = Vec3(0.0f, 2.0f, 0.0f);
	m_localVerts[10].m_position = Vec3(-3.0f, 1.f, 0.0f);
	m_localVerts[11].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[9].m_color = Rgba8(0, 255, 0);
	m_localVerts[10].m_color = Rgba8(0, 255, 0);
	m_localVerts[11].m_color = Rgba8(0, 255, 0);

	// Reversed Mid
	m_localVerts[12].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[13].m_position = Vec3(-3.0f, 1.f, 0.0f);
	m_localVerts[14].m_position = Vec3(-3.0f, -1.0f, 0.0f);
	m_localVerts[12].m_color = Rgba8(0, 255, 0);
	m_localVerts[13].m_color = Rgba8(0, 255, 0);
	m_localVerts[14].m_color = Rgba8(0, 255, 0);

	// Reversed bottom
	m_localVerts[15].m_position = Vec3(-3.0f, -1.0f, 0.0f);
	m_localVerts[16].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[17].m_position = Vec3(0.0f, -2.0f, 0.0f);
	m_localVerts[15].m_color = Rgba8(0, 255, 0);
	m_localVerts[16].m_color = Rgba8(0, 255, 0);
	m_localVerts[17].m_color = Rgba8(0, 255, 0);

	// Top wing spawner left triangle
	m_localVerts[18].m_position = Vec3(0.0f, 2.0f, 0.0f);
	m_localVerts[19].m_position = Vec3(0.0f, 4.f, 0.0f);
	m_localVerts[20].m_position = Vec3(1.5f, 4.0f, 0.0f);
	m_localVerts[18].m_color = Rgba8(0, 255, 0);
	m_localVerts[19].m_color = Rgba8(0, 255, 0);
	m_localVerts[20].m_color = Rgba8(0, 255, 0);

	// Top wing spawner right triangle
	m_localVerts[21].m_position = Vec3(1.5f, 4.0f, 0.0f);
	m_localVerts[22].m_position = Vec3(1.5f, 1.5f, 0.0f);
	m_localVerts[23].m_position = Vec3(0.0f, 2.0f, 0.0f);
	m_localVerts[21].m_color = Rgba8(0, 255, 0);
	m_localVerts[22].m_color = Rgba8(0, 255, 0);
	m_localVerts[23].m_color = Rgba8(0, 255, 0);

	// Bottom wing spawner left triangle
	m_localVerts[24].m_position = Vec3(0.0f, -2.0f, 0.0f);
	m_localVerts[25].m_position = Vec3(0.0f, -4.f, 0.0f);
	m_localVerts[26].m_position = Vec3(1.5f, -4.0f, 0.0f);
	m_localVerts[24].m_color = Rgba8(0, 255, 0);
	m_localVerts[25].m_color = Rgba8(0, 255, 0);
	m_localVerts[26].m_color = Rgba8(0, 255, 0);

	// Bottom wing spawner right triangle
	m_localVerts[27].m_position = Vec3(1.5f, -4.0f, 0.0f);
	m_localVerts[28].m_position = Vec3(1.5f, -1.5f, 0.0f);
	m_localVerts[29].m_position = Vec3(0.0f, -2.0f, 0.0f);
	m_localVerts[27].m_color = Rgba8(0, 255, 0);
	m_localVerts[28].m_color = Rgba8(0, 255, 0);
	m_localVerts[29].m_color = Rgba8(0, 255, 0);

	// Boss Beetle top eye
	m_localVerts[30].m_position = Vec3(3.5f, 0.4f, 0.0f);
	m_localVerts[31].m_position = Vec3(3.0f, 0.6f, 0.0f);
	m_localVerts[32].m_position = Vec3(3.0f, 0.2f, 0.0f);
	m_localVerts[30].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[31].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[32].m_color = Rgba8(255, 0, 0, 255);

	// Boss Beetle Bottom eye
	m_localVerts[33].m_position = Vec3(3.5f, -0.4f, 0.0f);
	m_localVerts[34].m_position = Vec3(3.0f, -0.6f, 0.0f);
	m_localVerts[35].m_position = Vec3(3.0f, -0.2f, 0.0f);
	m_localVerts[33].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[34].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[35].m_color = Rgba8(255, 0, 0, 255);

	// Top pincer
	m_localVerts[36].m_position = Vec3(3.0f, 0.75f, 0.0f);
	m_localVerts[37].m_position = Vec3(3.0f, 1.f, 0.0f);
	m_localVerts[38].m_position = Vec3(5.0f, 1.0f, 0.0f);
	m_localVerts[36].m_color = Rgba8(0, 255, 0);
	m_localVerts[37].m_color = Rgba8(0, 255, 0);
	m_localVerts[38].m_color = Rgba8(0, 255, 0);

	// Bottom pincer
	m_localVerts[39].m_position = Vec3(3.0f, -0.75f, 0.0f);
	m_localVerts[40].m_position = Vec3(3.0f, -1.f, 0.0f);
	m_localVerts[41].m_position = Vec3(5.0f, -1.0f, 0.0f);
	m_localVerts[38].m_color = Rgba8(0, 255, 0);
	m_localVerts[40].m_color = Rgba8(0, 255, 0);
	m_localVerts[41].m_color = Rgba8(0, 255, 0);
}

void BossBeetle::BossBeetleMovement()
{
	if (m_playerShip->IsAlive())
	{
		Vec2 playerPosition = m_playerShip->GetPosition();
		Vec2 bossBeetleDirection = playerPosition - m_position;
		float bossBeetleAngle = Atan2Degrees(bossBeetleDirection.y, bossBeetleDirection.x);

		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, bossBeetleAngle, 1.0f);
	}
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, BOSSBEETLE_SPEED);
}

void BossBeetle::SpawnBeetles()
{
	Vec2 leftWingPosition = m_position + Vec2(-1.5f * 7.5f, 4.0f * 7.5f); 
	Vec2 rightWingPosition = m_position + Vec2(1.5f * 7.5f, 4.0f * 7.5f); 

	float spawnDir = m_orientationDegrees;

	m_game->SpawnRandomBeetle(leftWingPosition, spawnDir); 
	m_game->SpawnRandomBeetle(rightWingPosition, spawnDir); 
}

void BossBeetle::TakeHit() 
{
	m_health -= 1;
	m_isHit = true;
	m_hitFlash = m_hitLength;
	if (m_health <= 0) 
	{
		BossBeetleDie();
	}
}