#pragma once
#include "Engine/Math/RandomNumberGenerator.h"

class App;
class Game;
class Renderer;
class InputSystem;
class AudioSystem;
class Window;
struct Vec2;
struct Rgba8;

constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 120;
constexpr int MAX_BULLETS = 500;
constexpr int MAX_BEETLES = 80;
constexpr int MAX_BOSSBEETLES = 10;
constexpr int MAX_WASPS = 50;
constexpr int NUM_WAVES = 50;
constexpr int MAX_DEBRIS = 100;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;

constexpr float WORLD_SIZE_X = 1000.f;
constexpr float WORLD_SIZE_Y = 2000.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float MINIWORLD_SIZE_X = 200.f;
constexpr float MINIWORLD_SIZE_Y = 100.f;
constexpr float MINIWORLD_CENTER_X = MINIWORLD_SIZE_X / 2.f;
constexpr float MINIWORLD_CENTER_Y = MINIWORLD_SIZE_Y / 2.f;

constexpr float MINIMAP_WIDTH = 400.f;
constexpr float MINIMAP_HEIGHT = 200.f;
constexpr float MINIMAP_SCALE_X = MINIMAP_WIDTH / WORLD_SIZE_X;
constexpr float MINIMAP_SCALE_Y = MINIMAP_HEIGHT / WORLD_SIZE_Y;

constexpr float SCREEN_SHAKE_REDUCTION_PER_SECOND = 1.0f;
constexpr float MAX_SCREEN_SHAKE = 3.f;

constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;

constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 150.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;
constexpr float BULLET_ARC_FIRE = 10.f;
constexpr int	BULLET_TOTAL_ANNIHILATIONMODE = 20;
constexpr float BULLET_DELAY = 0.1f;

constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr float PLAYER_SHIP_NOSE = 2.0f;

constexpr float BEETLE_SPEED = 10.f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.6f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;

constexpr float BOSSBEETLE_SPEED = 7.f;
constexpr float BOSSBEETLE_PHYSICS_RADIUS = 20.0f;
constexpr float BOSSBEETLE_COSMETIC_RADIUS = 30.0f;

constexpr float WASP_ACCELERATION = 10.f;
constexpr float WASP_SPEED = 32.f;
constexpr float WASP_PHYSICS_RADIUS = 1.6f;
constexpr float WASP_COSMETIC_RADIUS = 2.0f;

constexpr float DEBRIS_PHYSICS_RADIUS = 0.5f;
constexpr float DEBRIS_COSMETIC_RADIUS = 3.0f;

constexpr int STARTTRIANGLE_VERTS = 3;
constexpr int THRUST_TRIANGLE_VERTS = 3;
constexpr int MINIMAP_VERTS = 24;

constexpr int NUM_STARS = 1000;
constexpr int STAR_VERTS = 3;

extern App* g_theApp;
extern Game* g_theGame;
extern Renderer* g_theRenderer;
extern RandomNumberGenerator* g_rng;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern Window* g_theWindow;


void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
