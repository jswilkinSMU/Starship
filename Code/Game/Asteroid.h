#pragma once
#include "Entity.h"
#include "GameCommon.h"

#include <Engine/Core/Vertex_PCU.h>

constexpr int NUM_ASTEROID_SIDES = 16;
constexpr int NUM_ASTEROID_TRIS = NUM_ASTEROID_SIDES;
constexpr int NUM_ASTEROID_VERTS = 3 * NUM_ASTEROID_TRIS;

class Asteroid : public Entity
{
public:
	Asteroid(Game* owner, Vec2 const& startPos);
	~Asteroid();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void AsteroidDie();
	void TakeHit();

private:
	void InitializeLocalVerts();


private:
	Vertex_PCU m_localVerts[NUM_ASTEROID_VERTS];
};