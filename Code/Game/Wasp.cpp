#include "Wasp.h"
#include "Game.h"
#include <Engine/Core/VertexUtils.h>
#include "Engine/Renderer/Renderer.h"
#include <Engine/Math/MathUtils.h>
#include <Engine/Audio/AudioSystem.hpp>

Wasp::Wasp(Game* owner, Vec2 const& startPos, Entity* PlayerShip)
	:Entity(owner, startPos), m_playerShip(PlayerShip)
{
	m_health = 3;
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

Wasp::~Wasp()
{
}

void Wasp::Update(float deltaSeconds)
{
	WaspMovement();
	WaspThrust(deltaSeconds);
	GetClamped(WASP_ACCELERATION, 0.0f, 50.0f);
	m_position += (m_velocity * deltaSeconds);
}

void Wasp::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_WASP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_WASP_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_WASP_VERTS, tempWorldVerts);
}

void Wasp::WaspDie()
{
	m_isDead = true;
	m_isGarbage = true;
	SoundID enemyDefeatSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDefeat.mp3");
	g_theAudio->StartSound(enemyDefeatSound);
	m_game->SpawnDebrisCluster(7, m_position, m_velocity, 90.0f, 1.f, Rgba8(255, 255, 0));
}

void Wasp::TakeHit()
{
	m_health -= 1;
	if (m_health <= 0)
	{
		WaspDie();
	}
}

void Wasp::InitializeLocalVerts()
{
	// Wasp nose
	m_localVerts[0].m_position = Vec3(2.5f, 0.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(0.0f, 2.f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, -2.f, 0.0f);

	//Wasp tail
	m_localVerts[3].m_position = Vec3(0.0f, -1.f, 0.0f);
	m_localVerts[4].m_position = Vec3(0.0f, 1.f, 0.0f);
	m_localVerts[5].m_position = Vec3(-1.5f, 0.0f, 0.0f);

	// Wasp yellow color
	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(255, 255, 0);
	}
}

void Wasp::WaspMovement()
{
	Vec2 playerPosition = m_playerShip->GetPosition();
	//float playerRadius = m_playerShip->m_cosmeticRadius;
	Vec2 waspDirection = playerPosition - m_position;
	float waspAngle = Atan2Degrees(waspDirection.y, waspDirection.x);

	m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, waspAngle, 1.0f);
	//m_orientationDegrees = waspAngle;
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, WASP_SPEED);
}

void Wasp::WaspThrust(float deltaSeconds)
{
	Vec2 playerPosition = m_playerShip->GetPosition();
	Vec2 accel = playerPosition * WASP_ACCELERATION;
	m_velocity += accel * deltaSeconds;
}
