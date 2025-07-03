#pragma once

#include "Entity.h"
#include "GameCommon.h"

#include <Engine/Core/Vertex_PCU.h>

constexpr int NUM_DEBRIS_SIDES = 8;
constexpr int NUM_DEBRIS_TRIS = NUM_DEBRIS_SIDES; //temporary
constexpr int NUM_DEBRIS_VERTS = 3 * NUM_DEBRIS_TRIS;

class Debris : public Entity
{
public:
	Debris(Game* owner, Vec2 const& startPos, Vec2 const& averageVelocity, float radius, float maxScatterSpeed, const Rgba8& color);
	~Debris();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	Rgba8 m_color;

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_DEBRIS_VERTS];
	float m_ageInSeconds = 0.0f;
};