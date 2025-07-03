#include "Entity.h"
#include "Game.h"
#include "PlayerShip.h"
#include "GameCommon.h"
#include "Engine/Core/Rgba8.h"
#include <Engine/Audio/AudioSystem.hpp>



Entity::Entity(Game* owner, Vec2 const& startPos)
	:m_game(owner),
	m_position(startPos)
{
}

Entity::~Entity()
{
}

bool Entity::IsOffScreen() const
{
    if (m_position.x < -m_cosmeticRadius)
    {
        return true;
    }
    if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
    {
        return true;
    }
    if (m_position.y < -m_cosmeticRadius)
    {
        return true;
    }
    if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
    {
        return true;
    }
    return false;
}

bool Entity::IsAlive() const
{
	return !m_isDead;
}

bool Entity::IsDead() const
{
    return m_isDead;
}

bool Entity::IsGarbage() const
{
    return m_isGarbage;
}

Vec2 Entity::GetPosition() const
{
    return m_position;
}

Vec2 Entity::GetVelocity() const
{
    return m_velocity;
}

float Entity::GetCosmeticRadius() const
{
     return m_cosmeticRadius;
}

float Entity::GetPhysicsRadius() const
{
    return m_physicsRadius;
}

void Entity::DebugRender() const
{
	if (IsDead()) return;

	Vec2 fwd = Vec2::MakeFromPolarDegrees(m_orientationDegrees, 3.0f);
	Vec2 left = fwd.GetRotated90Degrees();

	if (m_game->GetPlayerShip())
	{
		DebugDrawLine(m_position, m_game->GetPlayerShip()->GetPosition(), 0.2f, Rgba8(50, 50, 50));
	}

	DebugDrawLine(m_position, m_position + fwd, 0.2f, Rgba8(255, 0, 0));
	DebugDrawLine(m_position, m_position + left, 0.2f, Rgba8(0, 255, 0));
	DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(255, 0, 255));
	DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(0, 255, 255));
	DebugDrawLine(m_position, m_position + m_velocity, 0.2f, Rgba8(255, 255, 0));
}

int Entity::EntityHealth()
{
    return m_health = 0;
}

void Entity::Die()
{
    m_isDead = true;
}

void Entity::Garbage()
{
    m_isGarbage = true;
}

void Entity::Alive()
{
    m_isDead = false;
}

