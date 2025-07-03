#include "Asteroid.h"
#include "Game.h"
#include "GameCommon.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/VertexUtils.h"
#include <Engine/Math/MathUtils.h>
#include <Engine/Audio/AudioSystem.hpp>

Asteroid::Asteroid(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos)
{
	m_health = 3;
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;

	// Asteroid movement
	m_angularVelocity = g_rng->RollRandomFloatInRange(-300.f, 300.f);
	m_orientationDegrees = g_rng->RollRandomFloatInRange(0.f, 360.f);
	float driftAngleDegrees = g_rng->RollRandomFloatInRange(0.f, 360.f);
	m_velocity.x = ASTEROID_SPEED * CosDegrees(driftAngleDegrees);
	m_velocity.y = ASTEROID_SPEED * SinDegrees(driftAngleDegrees);

	InitializeLocalVerts();

}

Asteroid::~Asteroid()
{
}

void Asteroid::Update(float deltaSeconds)
{
	// Euler integration
	m_position += (m_velocity * deltaSeconds);
	m_orientationDegrees += (m_angularVelocity * deltaSeconds);

	if (IsOffScreen())
	{
		if (m_position.x < -m_cosmeticRadius)
		{
			m_position.x = WORLD_SIZE_X + m_cosmeticRadius;
		}
		if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
		{
			m_position.x = -m_cosmeticRadius;
		}
		if (m_position.y < -m_cosmeticRadius)
		{
			m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;
		}
		if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
		{
			m_position.y = -m_cosmeticRadius;
		}
	}
}

void Asteroid::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_ASTEROID_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_ASTEROID_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_ASTEROID_VERTS, tempWorldVerts);
}

void Asteroid::AsteroidDie()
{
	m_isDead = true;
	m_isGarbage = true;
	SoundID enemyDefeatSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyDefeat.mp3");
	g_theAudio->StartSound(enemyDefeatSound);
	m_game->SpawnDebrisCluster(7, m_position, m_velocity, 90.0f, 1.f, Rgba8(100, 100, 100));
}

void Asteroid::TakeHit()
{
	m_health -= 1;
	if (m_health <= 0)
	{
		AsteroidDie();
	}
}

void Asteroid::InitializeLocalVerts()
{
	float asteroidRadii[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; ++sideNum)
	{
		asteroidRadii[sideNum] = g_rng->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	}

	constexpr float degreesPerAsteroidSide = 360.f / (float)NUM_ASTEROID_SIDES;
	Vec2 asteroidLocalVertPositions[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; ++sideNum)
	{
		float degrees = degreesPerAsteroidSide * (float)sideNum;
		float radius = asteroidRadii[sideNum];
		asteroidLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		asteroidLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	//Build triangles
	for (int triNum = 0; triNum < NUM_ASTEROID_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_ASTEROID_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = asteroidLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[endRadiusIndex];
		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	//Add color
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(100, 100, 100);
	}

}


