#pragma once
#include "Entity.h"
#include "GameCommon.h"

#include "Engine/Core/Vertex_PCU.h"

//----------------------------------------------------
constexpr int NUM_BULLET_TRIS = 2;
constexpr int NUM_BULLET_VERTS = 3 * NUM_BULLET_TRIS;

class Bullet : public Entity
{
public:
	Bullet(Game* owner, Vec2 const& startPos, float forwardDegrees);
	~Bullet();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void BulletDie();

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_BULLET_VERTS];
};