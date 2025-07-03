#include "Bullet.h"
#include "Game.h"

#include <Engine/Core/VertexUtils.h>
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/Vec2.hpp"
#include <Engine/Math/MathUtils.h>

Bullet::Bullet(Game* owner, Vec2 const& startPos, float forwardDegrees)
	:Entity(owner, startPos)
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_orientationDegrees = forwardDegrees;
	m_velocity.x = BULLET_SPEED * CosDegrees(forwardDegrees);
	m_velocity.y = BULLET_SPEED * SinDegrees(forwardDegrees);
	InitializeLocalVerts();

	Vec2 position = m_position;
}

Bullet::~Bullet()
{
}

void Bullet::Update(float deltaSeconds)
{
	// Euler integration
	m_position += (m_velocity * deltaSeconds);

	if ( IsOffScreen() ) 
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Bullet::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_BULLET_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_BULLET_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_BULLET_VERTS, tempWorldVerts);
}

void Bullet::BulletDie()
{
	m_isDead = true;
	m_isGarbage = true;
	m_game->SpawnDebrisCluster(3, m_position, m_velocity, 180.0f, 0.05f, Rgba8(255, 255, 0, 255));
}

void Bullet::InitializeLocalVerts()
{
	// Bullet nose
	m_localVerts[0].m_position = Vec3(0.5f, 0.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(0.0f, 0.5f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, -0.5f, 0.0f);
	m_localVerts[0].m_color = Rgba8(255, 255, 255, 255);
	m_localVerts[1].m_color = Rgba8(255, 255, 0, 255);
	m_localVerts[2].m_color = Rgba8(255, 255, 0, 255);

	// Bullet tail
	m_localVerts[3].m_position = Vec3(0.0f, -0.5f, 0.0f);
	m_localVerts[4].m_position = Vec3(0.0f, 0.5f, 0.0f);
	m_localVerts[5].m_position = Vec3(-2.0f, 0.0f, 0.0f);
	m_localVerts[3].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[4].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[5].m_color = Rgba8(255, 0, 0, 0);

}
