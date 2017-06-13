//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
	delete m_particleRenderer;
	delete m_particleWorld;
	for(Particle* particle : m_particles)
	{
		delete particle;
	}
	for (ParticleForceGenerator* forceGenerator : m_particleForceGenerators)
	{
		delete forceGenerator;
	}
	for (ParticleContactGenerator* contactGenerator : m_particleContactGenerators)
	{
		delete contactGenerator;
	}
	for (Platform* platform : m_platforms)
	{
		delete platform;
	}
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	
	srand(time(nullptr));
	m_particleWorld = new ParticleWorld(50000);
	ParticleForceRegistry& m_particleForceRegistry = m_particleWorld->GetForceRegistry();
	m_particleRenderer = new ParticleRenderer(Colors::White);
	m_particleRenderer->Initialize(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
	Vector3 gravity = Vector3::Down * 20;
	for (int i = 1; i <= 10; ++i)
	{
		Particle* particle = new Particle();
		particle->SetPosition(Vector3::UnitX * static_cast<float>(i)*10.f);
		particle->SetMass(static_cast<float>(i));
		particle->SetVelocity(Vector3::Up * i *(rand() % 20) + Vector3::Left * i *(rand() % 10));
		particle->SetAcceleration(gravity);
		particle->SetWorldSpaceRadius(static_cast<float>(i) / 2.f);
		m_particles.push_back(particle);
	}

	m_particleAnchor[0] = Vector3(-50, 50, 0);
	m_particleAnchor[1] = Vector3(0, 50, 0);
	m_particleAnchor[2] = Vector3(50, 50, 0);
	
	float springConstant = 10.f;
	float restLength = 2;
	float damping = 0.8f;
	ParticleAnchoredBungeeForceGenerator* anchoredBungeeForceGenerator = new ParticleAnchoredBungeeForceGenerator(&m_particleAnchor[0], springConstant, restLength);
	ParticleAnchoredFakeStiffSpringForceGenerator* anchoredFakeStiffSpringForceGenerator = new ParticleAnchoredFakeStiffSpringForceGenerator(&m_particleAnchor[1], springConstant, damping);
	ParticleAnchoredSpringForceGenerator* anchoredSpringForceGenerator = new ParticleAnchoredSpringForceGenerator(&m_particleAnchor[2], springConstant, restLength);
	m_particleForceGenerators.push_back(anchoredBungeeForceGenerator);
	m_particleForceGenerators.push_back(anchoredFakeStiffSpringForceGenerator);
	m_particleForceGenerators.push_back(anchoredSpringForceGenerator);

	Particle* anchoredBungeeParticle = new Particle();
	anchoredBungeeParticle->SetPosition(m_particleAnchor[0]);
	anchoredBungeeParticle->SetMass(10);
	anchoredBungeeParticle->SetAcceleration(gravity);
	anchoredBungeeParticle->SetWorldSpaceRadius(5);
	m_particles.push_back(anchoredBungeeParticle);
	m_particleForceRegistry.Add(anchoredBungeeParticle, anchoredBungeeForceGenerator);

	Particle* anchoredFakeStiffSpringParticle = new Particle();
	anchoredFakeStiffSpringParticle->SetPosition(m_particleAnchor[1]+Vector3::Down*50);
	anchoredFakeStiffSpringParticle->SetMass(10);
	anchoredFakeStiffSpringParticle->SetAcceleration(gravity);
	anchoredFakeStiffSpringParticle->SetWorldSpaceRadius(5);
	m_particles.push_back(anchoredFakeStiffSpringParticle);
	m_particleForceRegistry.Add(anchoredFakeStiffSpringParticle, anchoredFakeStiffSpringForceGenerator);

	Particle* anchoredSpringParticle = new Particle();
	anchoredSpringParticle->SetPosition(m_particleAnchor[2]);
	anchoredSpringParticle->SetMass(10);
	anchoredSpringParticle->SetAcceleration(gravity);
	anchoredSpringParticle->SetWorldSpaceRadius(5);
	m_particles.push_back(anchoredSpringParticle);
	m_particleForceRegistry.Add(anchoredSpringParticle, anchoredSpringForceGenerator);


	ParticleBungeeForceGenerator* bungeeForceGenerator = new ParticleBungeeForceGenerator(anchoredBungeeParticle, springConstant/2.f, restLength/2.f);
	ParticleSpringForceGenerator* springForceGenerator = new ParticleSpringForceGenerator(anchoredSpringParticle, springConstant/2.f, restLength/2.f);
	m_particleForceGenerators.push_back(bungeeForceGenerator);
	m_particleForceGenerators.push_back(springForceGenerator);

	Particle* bungeeParticle = new Particle();
	bungeeParticle->SetPosition(m_particleAnchor[0] + Vector3::UnitX * 25);
	bungeeParticle->SetMass(10);
	bungeeParticle->SetAcceleration(gravity);
	bungeeParticle->SetWorldSpaceRadius(5);
	m_particles.push_back(bungeeParticle);
	m_particleForceRegistry.Add(bungeeParticle, bungeeForceGenerator);

	Particle* springParticle = new Particle();
	springParticle->SetPosition(m_particleAnchor[2] + Vector3::UnitX * 25);
	springParticle->SetMass(10);
	springParticle->SetAcceleration(gravity);
	springParticle->SetWorldSpaceRadius(5);
	m_particles.push_back(springParticle);
	m_particleForceRegistry.Add(springParticle, springForceGenerator);

	m_particleWorld->AddParticle(m_particles);
	m_particleRenderer->AddParticle(m_particles);


	// ---------------------------- LEVEL BOUND PLATFORMS ----------------------------
	Vector2 windowHalf = Vector2(width / 2.f, height / 2.f);

	Vector3 levelBoundPlatformStartEnd[4][2] = {
		{ Vector3(-windowHalf.x, -windowHalf.y, 0), Vector3(windowHalf.x, -windowHalf.y, 0) },	// ground
		{ Vector3(windowHalf.x, windowHalf.y, 0), Vector3(-windowHalf.x, windowHalf.y, 0) },	// ceiling
		{ Vector3(-windowHalf.x, windowHalf.y, 0), Vector3(-windowHalf.x, -windowHalf.y, 0) },	// left wall
		{ Vector3(windowHalf.x, -windowHalf.y, 0), Vector3(windowHalf.x, windowHalf.y, 0) }	// right wall
	};
	for(int i = 0; i < 4; ++i)
	{
		Platform* platform = new Platform();
		platform->SetColorAndThickness(Colors::Blue, 1);
		platform->Initialize(levelBoundPlatformStartEnd[i][0], levelBoundPlatformStartEnd[i][1], m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
		m_platforms.push_back(platform);

		ParticlePlatformContactsGenerator* platformContactsGenerator = new ParticlePlatformContactsGenerator(levelBoundPlatformStartEnd[i][0], levelBoundPlatformStartEnd[i][1]);
		platformContactsGenerator->AddParticle(m_particles);
		m_particleContactGenerators.push_back(platformContactsGenerator);
		m_particleWorld->GetContactGenerators().push_back(platformContactsGenerator);
	}
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});
	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.
	m_particleWorld->StartFrame();

	auto kb = m_keyboard->GetState();
	if (kb.Escape)
		PostQuitMessage(0);

	checkAndProcessKeyboardInput(elapsedTime);
	checkAndProcessMouseInput(elapsedTime);

	m_particleWorld->RunPhysics(elapsedTime);

	m_camera.UpdateViewMatrix();
}

void Game::checkAndProcessKeyboardInput(const float& deltaTime)
{
	auto kb = m_keyboard->GetState();
	if (kb.Escape)
		PostQuitMessage(0);
}

void Game::checkAndProcessMouseInput(const float& deltaTime)
{
	auto mouse = m_mouse->GetState();
}

#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	// TODO: Add your rendering code here.
	context->ClearRenderTargetView(m_deviceResources->GetRenderTargetView(), Colors::Black);
	//context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	//context->RSSetState(m_states->CullNone());

	//m_effect->SetMatrices(Matrix::CreateScale(10), m_camera.GetView(), m_camera.GetProj());
	//m_effect->Apply(context);

	//context->IASetInputLayout(m_inputLayout.Get());

	//m_batch->Begin();

	//VertexPositionColor v1(Vector3(0.f, 0.5f, 0.5f), Colors::Yellow);
	//VertexPositionColor v2(Vector3(0.5f, -0.5f, 0.5f), Colors::Yellow);
	//VertexPositionColor v3(Vector3(-0.5f, -0.5f, 0.5f), Colors::Yellow);

	//m_batch->DrawTriangle(v1, v2, v3);

	//m_batch->End();
	m_particleRenderer->Render(context, m_camera);
	for(Platform* platform : m_platforms)
	{
		platform->Render(context, m_camera);
	}
	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();
	auto deviceContext = m_deviceResources->GetD3DDeviceContext();

	// TODO: Initialize device dependent objects here (independent of window size).
	m_camera.SetPosition(0.0f, 0.0f, -1.f);
	m_camera.LookAt(Vector3::Up, Vector3(0, 0, 0) - m_camera.GetPosition());

	m_states = std::make_unique<CommonStates>(device);

	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(
		device->CreateInputLayout(VertexPositionColor::InputElements,
		                          VertexPositionColor::InputElementCount,
		                          shaderByteCode, byteCodeLength,
		                          m_inputLayout.ReleaseAndGetAddressOf()));

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_deviceResources->GetD3DDeviceContext());

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto width = m_deviceResources->GetScreenViewport().Width;
	auto height = m_deviceResources->GetScreenViewport().Height;

	// TODO: Initialize windows-size dependent objects here.
	m_camera.SetOrthographicLens(width, height, 0.f, 100.f);
	m_camera.UpdateViewMatrix();
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
