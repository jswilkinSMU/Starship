#include "Debris.h"
#include <Engine/Core/VertexUtils.h>
#include "Engine/Renderer/Renderer.h"
#include <Engine/Math/MathUtils.h>

Debris::Debris(Game* owner, Vec2 const& startPos, Vec2 const& averageVelocity, float radius, float maxScatterSpeed, const Rgba8& color)
	:Entity(owner, startPos)
{

	m_physicsRadius = radius * 0.5f;
	m_cosmeticRadius = radius * 1.5f;
	m_angularVelocity = g_rng->RollRandomFloatInRange(-200.f, 200.f);
	float randomSprayAngle = g_rng->RollRandomFloatInRange(-maxScatterSpeed, maxScatterSpeed);
	Vec2 debrisVelocity = averageVelocity.GetRotatedDegrees(randomSprayAngle);
	m_velocity = debrisVelocity;
	g_rng->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	m_color = color;
	InitializeLocalVerts();
}



Debris::~Debris()
{
}

void Debris::Update(float deltaSeconds)
{
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
	m_position += m_velocity * deltaSeconds;

	m_ageInSeconds += deltaSeconds;
	float alpha = RangeMapClamped(m_ageInSeconds, 0, 2, 127, 0);
	Rgba8 colorNow = m_color;
	colorNow.a = static_cast<unsigned char>(alpha);
	m_color = colorNow;

	if (m_ageInSeconds >= 2.0f)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Debris::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_DEBRIS_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
		tempWorldVerts[vertIndex].m_color = m_color;
	}
	TransformVertexArrayXY3D(NUM_DEBRIS_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_DEBRIS_VERTS, tempWorldVerts);
}

void Debris::InitializeLocalVerts()
{
	float debrisRadii[NUM_DEBRIS_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; ++sideNum)
	{
		debrisRadii[sideNum] = g_rng->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	}

	constexpr float degreesPerDEBRISSide = 360.f / (float)NUM_DEBRIS_SIDES;
	Vec2 DEBRISLocalVertPositions[NUM_DEBRIS_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; ++sideNum)
	{
		float degrees = degreesPerDEBRISSide * (float)sideNum;
		float radius = debrisRadii[sideNum];
		DEBRISLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		DEBRISLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	//Build triangles
	for (int triNum = 0; triNum < NUM_DEBRIS_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_DEBRIS_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = DEBRISLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = DEBRISLocalVertPositions[endRadiusIndex];
		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	//Add color
	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(100, 100, 100 );
	}
}
