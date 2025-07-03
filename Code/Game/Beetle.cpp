#include "Beetle.h"
#include "Game.h"
#include <Engine/Core/VertexUtils.h>
#include "Engine/Renderer/Renderer.h"
#include <Engine/Math/MathUtils.h>
#include <Engine/Audio/AudioSystem.hpp>

Beetle::Beetle(Game* owner, Vec2 const& startPos, float forwardDegrees, Entity* PlayerShip)
	: Entity(owner, startPos), m_playerShip(PlayerShip)
{
	m_health = 3;
	m_orientationDegrees = forwardDegrees;
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

Beetle::~Beetle()
{
}

void Beetle::Update(float deltaSeconds)
{
	BeetleMovement();
	m_position += (m_velocity * deltaSeconds);

	if (IsOffScreen())
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Beetle::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_BEETLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_BEETLE_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_BEETLE_VERTS, tempWorldVerts);
}

void Beetle::BeetleDie()
{
	m_isDead = true;
	m_isGarbage = true;
	SoundID enemyDefeatSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDefeat.mp3");
	g_theAudio->StartSound(enemyDefeatSound);
	m_game->SpawnDebrisCluster(7, m_position, m_velocity, 90.0f, 1.f, Rgba8(0, 255, 0));
}

void Beetle::TakeHit()
{
	m_health -= 1;
	if (m_health <= 0)
	{
		BeetleDie();
	}
}


void Beetle::InitializeLocalVerts()
{
	// Beetle top
	m_localVerts[0].m_position = Vec3(0.0f, 2.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(3.0f, 1.f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, 0.0f, 0.0f);

	// Beetle mid
	m_localVerts[3].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[4].m_position = Vec3(3.0f, 1.f, 0.0f);
	m_localVerts[5].m_position = Vec3(3.0f, -1.0f, 0.0f);

	// Beetle bottom
	m_localVerts[6].m_position = Vec3(3.0f, -1.0f, 0.0f);
	m_localVerts[7].m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_localVerts[8].m_position = Vec3(0.0f, -2.0f, 0.0f);

	// Beetle green color
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(0, 255, 0);
	}
}

void Beetle::BeetleMovement()
{
	if (!m_playerShip->IsDead())
	{
		Vec2 playerPosition = m_playerShip->GetPosition();
		Vec2 beetleDirection = playerPosition - m_position;
		float beetleAngle = Atan2Degrees(beetleDirection.y, beetleDirection.x);

		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, beetleAngle, 1.0f);
	}
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, BEETLE_SPEED);
}
