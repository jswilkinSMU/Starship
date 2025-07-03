#pragma once

#include "Entity.h"
#include "GameCommon.h"

#include <Engine/Core/Vertex_PCU.h>

constexpr int NUM_BEETLE_TRIS = 3;
constexpr int NUM_BEETLE_VERTS = 3 * NUM_BEETLE_TRIS;

class Beetle : public Entity
{
public:
	Beetle(Game* owner, Vec2 const& startPos, float forwardDegrees, Entity* PlayerShip);
	~Beetle();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void BeetleDie();
	void TakeHit();

private:
	void InitializeLocalVerts();
	void BeetleMovement();

private:
	Vertex_PCU m_localVerts[NUM_BEETLE_VERTS];
	Entity* m_playerShip;
};