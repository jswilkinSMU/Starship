#pragma once
#include "Entity.h"
#include "GameCommon.h"

#include "Engine/Math/Vec2.hpp"
#include <Engine/Core/Vertex_PCU.h>


constexpr int NUM_SHIP_TRIS = 5;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;

class PlayerShip : public Entity
{

public:
	PlayerShip(Game* owner, Vec2 const& startPos);
	~PlayerShip();

	virtual void Update(float deltaSeconds) override;
	void UpdateFromController(float deltaSeconds);

	virtual void Render() const override;
	void Respawn();
	void PlayerShipDie();

	static void InitializeVerts(Vertex_PCU* vertstoFillIn);

private:
	void InitializeLocalVerts();
	void InitializeThrustVerts();
	void UpdateFromKeyboard(float deltaSeconds);

	void AllOutAssault();
	void BulletStream(float deltaSeconds);
	void AnnihilationMode();
	void BounceOffWalls();
	Vec2 GetForwardNormal() const;

private:
	Vertex_PCU thrustTriangle[THRUST_TRIANGLE_VERTS];
	Vertex_PCU m_localVerts[NUM_SHIP_VERTS];
	float m_thrustFraction = 0.0f;
	bool m_isThrusting = false;
	float m_ageInSeconds = 0.0f;

	bool m_bulletisStreaming = false; 
	float m_burstTime = 0.0f; 

	int m_bulletCounter = 0;
	int m_bulletsFired = 0; 
	const int m_totalBullets = 20; 

	const float m_timeBetweenShots = 0.1f; 

};