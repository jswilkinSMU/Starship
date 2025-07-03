#pragma once
#include "Game/Game.h"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.h"
#include "Engine/Core/EventSystem.hpp"

class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();

	void RunMainLoop();
	bool IsQuitting() const { return m_isQuitting; }
	static bool HandleQuitRequested(EventArgs& args);

	Clock m_gameClock;
private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

	void SubscribeToEvents();

private:
	float m_timeLastFrameStart = 0.0f;
	bool  m_isQuitting = false;
};