#pragma once

#include "Entity.h"
#include "GameCommon.h"

#include <Engine/Core/Vertex_PCU.h>

constexpr int NUM_WASP_TRIS = 2;
constexpr int NUM_WASP_VERTS = 3 * NUM_WASP_TRIS;

class Wasp : public Entity
{
public:
	Wasp(Game* owner, Vec2 const& startPos, Entity* PlayerShip);
	~Wasp();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void WaspDie();
	void TakeHit();

private:
	void InitializeLocalVerts();
	void WaspMovement();
	void WaspThrust(float deltaSeconds);

private:
	Vertex_PCU m_localVerts[NUM_WASP_VERTS];
	Entity* m_playerShip;
};