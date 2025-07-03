#pragma once
class Game;
#include "Engine/Math/Vec2.hpp"

class Entity
{
public:
	Entity(Game* owner, Vec2 const& startPos);
	virtual ~Entity();
	bool IsOffScreen() const;
	bool IsAlive() const;
	bool IsDead() const;
	bool IsGarbage() const;
	Vec2 GetPosition() const;
	Vec2 GetVelocity() const;
	float GetCosmeticRadius() const;
	float GetPhysicsRadius() const;
	int m_health = 1;

	Game* m_game = nullptr;

public:
	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual int  EntityHealth();
	virtual void Die();
	virtual void Garbage();
	virtual void Alive();

protected:
	//Universal data members
	Vec2 m_position;
	Vec2 m_velocity;
	float m_orientationDegrees = 0.f;
	float m_angularVelocity = 0.f;
	float m_physicsRadius = 5.f;
	float m_cosmeticRadius = 10.f;
	bool m_isDead = false;
	bool m_isGarbage = false;
};