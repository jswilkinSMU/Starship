#pragma once

#include "Entity.h"
#include "GameCommon.h"

#include <Engine/Core/Vertex_PCU.h>

constexpr int NUM_BOSSBEETLE_TRIS = 14;
constexpr int NUM_BOSSBEETLE_VERTS =  3 * NUM_BOSSBEETLE_TRIS;

class BossBeetle : public Entity
{
public:
	BossBeetle(Game* owner, Vec2 const& startPos, Entity* PlayerShip);
	~BossBeetle();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void BossBeetleDie();
	void TakeHit();

private:
	void InitializeLocalVerts();
	void BossBeetleMovement();
	void SpawnBeetles();

private:
	Vertex_PCU m_localVerts[NUM_BOSSBEETLE_VERTS];
	Entity* m_playerShip;

	float m_spawnTime = 0.0f;
	float m_spawnInterval = 20.0f; 

	float m_hitLength = 0.3f;
	float m_hitFlash = 0.0f; 
	bool m_isHit = false;
};