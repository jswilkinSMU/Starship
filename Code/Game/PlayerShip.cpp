#include "App.h"
#include "PlayerShip.h"
#include "GameCommon.h"

#include <Engine/Core/VertexUtils.h>
#include "Engine/Renderer/Renderer.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Input/AnalogJoystick.h"
#include <Engine/Math/MathUtils.h>
#include <Engine/Audio/AudioSystem.hpp>


PlayerShip::PlayerShip(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos) 

{
	PlayerShip::InitializeVerts(&m_localVerts[0]);
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

PlayerShip::~PlayerShip()
{
}

void PlayerShip::Update(float deltaSeconds)
{
	UpdateFromController(deltaSeconds);
	UpdateFromKeyboard(deltaSeconds);
	BounceOffWalls();
	BulletStream(deltaSeconds);
	m_position += (m_velocity * deltaSeconds);
}

void PlayerShip::UpdateFromController(float deltaSeconds)
{
	XboxController const& controller = g_theInput->GetController(0);
	const float innerDeadZone = 0.3f;
	const float outerDeadZone = 0.95f;
	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	float leftStickAngle = controller.GetLeftStick().GetOrientationDegrees();

	// Dead zone handling
	leftStickMagnitude = RangeMapClamped(leftStickMagnitude, innerDeadZone, outerDeadZone, 0.f, 1.f);

	if (g_theInput->IsKeyDown('E')) 
	{
		m_thrustFraction = 1.0f;
	}
	else 
	{
		m_thrustFraction = leftStickMagnitude;
		if (leftStickMagnitude > 0.f) 
		{
			m_orientationDegrees = leftStickAngle;
		}
	}

	// Driving
	Vec2 accel = GetForwardNormal() * PLAYER_SHIP_ACCELERATION * m_thrustFraction;
	m_velocity += accel * deltaSeconds;

	// Shooting
	if (controller.WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_A))
	{
		Vec2 bulletAtShipNose = GetForwardNormal() * PLAYER_SHIP_NOSE;
		Vec2 bulletPos = m_position + bulletAtShipNose;
		float bulletDir = m_orientationDegrees;

		m_game->SpawnBullet(bulletPos, bulletDir);
		SoundID bulletSound = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Bullet_Firing.wav");
		g_theAudio->StartSound(bulletSound, false, 0.2f, 0.5f, g_rng->RollRandomFloatInRange(0.1f, 1.0f), false);

		m_bulletCounter += 1;

		if (m_bulletCounter % 5 == 0) 
		{
			AllOutAssault();
		}

		if (m_bulletCounter % 15 == 0) 
		{
			if (g_rng->RollRandomChance(0.4f)) 
			{
				AnnihilationMode();
			}
		}
	}
}

void PlayerShip::Render() const
{
	if (m_isDead)
		return;
	
	Vertex_PCU tempShipWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, &tempShipWorldVerts[0]);

	if (m_isThrusting == true) 
	{
		Vertex_PCU tempThrustVerts[THRUST_TRIANGLE_VERTS];
		for (int vertIndex = 0; vertIndex < THRUST_TRIANGLE_VERTS; ++vertIndex) 
		{
			tempThrustVerts[vertIndex] = thrustTriangle[vertIndex];
		}
		TransformVertexArrayXY3D(THRUST_TRIANGLE_VERTS, tempThrustVerts, g_rng->RollRandomFloatInRange(0.9f, m_thrustFraction), m_orientationDegrees, m_position);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray(THRUST_TRIANGLE_VERTS, &tempThrustVerts[0]);
	}
}

void PlayerShip::InitializeThrustVerts()
{
	thrustTriangle[0].m_position = Vec3(-6.0f, 0.0f, 0.0f);
	thrustTriangle[0].m_color = Rgba8(255, 165, 0, 127);
	thrustTriangle[1].m_position = Vec3(-2.0f, 0.7f, 0.0f);
	thrustTriangle[1].m_color = Rgba8(139, 0, 0, 255);
	thrustTriangle[2].m_position = Vec3(-2.0f, -0.7f, 0.0f);
	thrustTriangle[2].m_color = Rgba8(139, 0, 0, 255);
}

void PlayerShip::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(1.f, 0.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(0.0f, 1.f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, -1.f, 0.0f);

	m_localVerts[3].m_position = Vec3(2.f, 1.f, 0.0f);
	m_localVerts[4].m_position = Vec3(0.0f, 2.f, 0.0f);
	m_localVerts[5].m_position = Vec3(-2.f, 1.f, 0.0f);

	m_localVerts[6].m_position = Vec3(2.f, -1.f, 0.0f);
	m_localVerts[7].m_position = Vec3(-2.f, -1.f, 0.0f);
	m_localVerts[8].m_position = Vec3(0.0f, -2.f, 0.0f);

	m_localVerts[9].m_position = Vec3(0.0f, 1.f, 0.0f);
	m_localVerts[10].m_position = Vec3(-2.f, 1.f, 0.0f);
	m_localVerts[11].m_position = Vec3(-2.0f, -1.f, 0.0f);

	m_localVerts[12].m_position = Vec3(0.0f, -1.f, 0.0f);
	m_localVerts[13].m_position = Vec3(-2.f, -1.f, 0.0f);
	m_localVerts[14].m_position = Vec3(0.f, 1.f, 0.0f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(102, 153, 204);
	}
}

void PlayerShip::UpdateFromKeyboard(float deltaSeconds)
{

	//Rotations
	if (g_theInput->IsKeyDown('F'))
	{
		m_orientationDegrees += (PLAYER_SHIP_TURN_SPEED * deltaSeconds);
	}
	if (g_theInput->IsKeyDown('S'))
	{
		m_orientationDegrees -= (PLAYER_SHIP_TURN_SPEED * deltaSeconds);
	}

	//Thrusting
	if (g_theInput->IsKeyDown('E')) 
	{
		m_isThrusting = true;
		m_thrustFraction += deltaSeconds;
		Vec2 accel = GetForwardNormal() * PLAYER_SHIP_ACCELERATION;
		m_velocity += accel * deltaSeconds;

		InitializeThrustVerts();
	}
	else 
	{
		m_isThrusting = false;
		m_thrustFraction = 0.0f; 
	}


	if (g_theInput->WasKeyJustPressed(' '))
	{
		Vec2 bulletAtShipNose = GetForwardNormal() * PLAYER_SHIP_NOSE;
		Vec2 bulletPos = m_position + bulletAtShipNose;
		float bulletDir = m_orientationDegrees;

		m_game->SpawnBullet(bulletPos, bulletDir);
		SoundID bulletSound = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Bullet_Firing.wav");
		g_theAudio->StartSound(bulletSound, false, 0.2f, 0.5f, g_rng->RollRandomFloatInRange(0.1f, 1.0f), false);

		m_bulletCounter += 1;

		if (m_bulletCounter % 5 == 0) 
		{
			AllOutAssault();
		}

		if (m_bulletCounter % 15 == 0) 
		{
			if (g_rng->RollRandomChance(0.6f)) 
			{
				AnnihilationMode();
			}
		}
	}
}

void PlayerShip::AllOutAssault()
{
	int numBullets = g_rng->RollRandomIntInRange(3, 8);
	Vec2 bulletAtShipNose = GetForwardNormal() * PLAYER_SHIP_NOSE;

	for (int bulletIndex = 0; bulletIndex < numBullets; ++bulletIndex)
	{
		float bulletSpacing = (bulletIndex - numBullets / 2.0f) * BULLET_ARC_FIRE;
		float bulletDir = m_orientationDegrees + bulletSpacing;

		Vec2 bulletPos = m_position + bulletAtShipNose;
		m_game->SpawnBullet(bulletPos, bulletDir);
	}
}

void PlayerShip::BulletStream(float deltaSeconds)
{
	if (m_bulletisStreaming == true)
	{
		m_burstTime += deltaSeconds;
		if (m_burstTime >= BULLET_DELAY)
		{
			m_burstTime -= BULLET_DELAY;

			Vec2 bulletAtShipNose = GetForwardNormal() * PLAYER_SHIP_NOSE;
			Vec2 bulletPos = m_position + bulletAtShipNose;

			float bulletDir = m_orientationDegrees;
			m_game->SpawnBullet(bulletPos, bulletDir);
			m_bulletsFired += 1;

			if (m_bulletsFired >= BULLET_TOTAL_ANNIHILATIONMODE)
			{
				m_bulletisStreaming = false;
				m_bulletsFired = 0;
			}
		}
	}
}

void PlayerShip::AnnihilationMode()
{
	m_bulletisStreaming = true;
	m_burstTime = 0.0f;
	m_bulletsFired = 0;
}

void PlayerShip::BounceOffWalls()
{
	//West wall
	if (m_position.x < m_physicsRadius)
	{
		m_position.x = m_physicsRadius;
		m_velocity.x *= -1.f;
	}

	//East wall
	if (m_position.x > WORLD_SIZE_X - m_physicsRadius)
	{
		m_position.x = WORLD_SIZE_X - m_physicsRadius;
		m_velocity.x *= -1.f;
	}

	//South wall
	if (m_position.y < m_physicsRadius)
	{
		m_position.y = m_physicsRadius;
		m_velocity.y *= -1.f;
	}

	//North Wall
	if (m_position.y > WORLD_SIZE_Y - m_physicsRadius)
	{
		m_position.y = WORLD_SIZE_Y - m_physicsRadius;
		m_velocity.y *= -1.f;
	}
}

Vec2 PlayerShip::GetForwardNormal() const
{
	return Vec2::MakeFromPolarDegrees(m_orientationDegrees);
}

void PlayerShip::Respawn()
{
	m_isDead = false;
	m_position = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
	m_velocity = Vec2(0.f, 0.f);
	m_orientationDegrees = 0.f;
	m_bulletCounter = 0;
}

void PlayerShip::PlayerShipDie()
{
	m_isDead = true;
	m_game->AddScreenShake(1.0f);
	m_game->SpawnDebrisCluster(15, m_position, m_velocity, 200.0f, 1.f, Rgba8(102, 153, 204));
	SoundID shipCrashSound = g_theAudio->CreateOrGetSound("Data/Audio/ShipCrash.mp3");
	g_theAudio->StartSound(shipCrashSound);
}

void PlayerShip::InitializeVerts(Vertex_PCU* vertstoFillIn)
{
	Vertex_PCU fakeShip[NUM_SHIP_VERTS];

	vertstoFillIn[0].m_position = Vec3(1.f, 0.0f, 0.0f);
	vertstoFillIn[1].m_position = Vec3(0.0f, 1.f, 0.0f);
	vertstoFillIn[2].m_position = Vec3(0.0f, -1.f, 0.0f);

	vertstoFillIn[3].m_position = Vec3(2.f, 1.f, 0.0f);
	vertstoFillIn[4].m_position = Vec3(0.0f, 2.f, 0.0f);
	vertstoFillIn[5].m_position = Vec3(-2.f, 1.f, 0.0f);

	vertstoFillIn[6].m_position = Vec3(2.f, -1.f, 0.0f);
	vertstoFillIn[7].m_position = Vec3(-2.f, -1.f, 0.0f);
	vertstoFillIn[8].m_position = Vec3(0.0f, -2.f, 0.0f);

	vertstoFillIn[9].m_position = Vec3(0.0f, 1.f, 0.0f);
	vertstoFillIn[10].m_position = Vec3(-2.f, 1.f, 0.0f);
	vertstoFillIn[11].m_position = Vec3(-2.0f, -1.f, 0.0f);

	vertstoFillIn[12].m_position = Vec3(0.0f, -1.f, 0.0f);
	vertstoFillIn[13].m_position = Vec3(-2.f, -1.f, 0.0f);
	vertstoFillIn[14].m_position = Vec3(0.f, 1.f, 0.0f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		vertstoFillIn[vertIndex].m_color = Rgba8(102, 153, 204);
	}

}



