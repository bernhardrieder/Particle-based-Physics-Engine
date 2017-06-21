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
	for (BlizzardParticleEmitter* emitter : m_blizzardParticleEmitter)
	{
		delete emitter;
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
	Vector2 cameraLevelBounds = Vector2(width / 2.f, height / 2.f);

	m_particleWorld = new ParticleWorld(50000, 5000, cameraLevelBounds);
	ParticleForceRegistry& m_particleForceRegistry = m_particleWorld->GetForceRegistry();
	m_particleRenderer = new ParticleRenderer(Colors::White);
	m_particleRenderer->Initialize(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext(), m_particleWorld);


	// ---------------------------- CLOTH ----------------------------
	{
		m_particleAnchor[0] = Vector3(-130, 200, 0);
		m_particleAnchor[1] = Vector3(-100, 200, 0);
		m_particleAnchor[2] = Vector3(-70, 200, 0);
	
		float springConstant = 50.f;
		float restLength = 10;
		float damping = 0.8f;

		Particle* particle[3][3];

		for(int x = 0; x < 3; ++x)
		{
			//ParticleAnchoredSpringForceGenerator* anchoredSpringForceGenerator = new ParticleAnchoredSpringForceGenerator(&m_particleAnchor[x], springConstant, restLength);
			ParticleAnchoredFakeStiffSpringForceGenerator* anchoredSpringForceGenerator = new ParticleAnchoredFakeStiffSpringForceGenerator(&m_particleAnchor[x], springConstant, damping);
			m_particleForceGenerators.push_back(anchoredSpringForceGenerator);

			Particle* anchoredSpringParticle = m_particleWorld->GetNewParticle();
			anchoredSpringParticle->SetPosition(m_particleAnchor[x]);
			anchoredSpringParticle->SetMass(10);
			anchoredSpringParticle->SetAcceleration(m_gravity);
			anchoredSpringParticle->SetWorldSpaceRadius(10);
			anchoredSpringParticle->SetType(ParticleTypes::Cloth);
			anchoredSpringParticle->SetBouncinessFactor(0.f);
			m_particleForceRegistry.Add(anchoredSpringParticle, anchoredSpringForceGenerator);


			particle[x][0] = anchoredSpringParticle;
			for(int y = 1; y < 3; ++y)
			{
				//ParticleSpringForceGenerator* gen1 = new ParticleSpringForceGenerator(particle[x][y - 1], springConstant / 2, restLength / 2);
				ParticleFakeStiffSpringForceGenerator* gen1 = new ParticleFakeStiffSpringForceGenerator(particle[x][y-1], springConstant/2, damping);
				m_particleForceGenerators.push_back(gen1);

				Particle* springParticle = m_particleWorld->GetNewParticle();
				springParticle->SetPosition(m_particleAnchor[x] + Vector3::Down * 30 * y);
				springParticle->SetMass(10);
				springParticle->SetAcceleration(m_gravity);
				springParticle->SetWorldSpaceRadius(10);
				springParticle->SetType(ParticleTypes::Cloth);
				springParticle->SetBouncinessFactor(0.f);
				m_particleForceRegistry.Add(springParticle, gen1);
				particle[x][y] = springParticle;


				//ParticleSpringForceGenerator* gen2 = new ParticleSpringForceGenerator(springParticle, springConstant / 2, restLength / 2);
				ParticleFakeStiffSpringForceGenerator* gen2 = new ParticleFakeStiffSpringForceGenerator(springParticle, springConstant/2, damping);
				m_particleForceRegistry.Add(particle[x][y - 1], gen2);
				m_particleForceGenerators.push_back(gen2);

			}
		}

		for(int y = 0; y < 3; ++y)
		{
			for(int x = 0; x < 2; ++x)
			{
				//ParticleSpringForceGenerator* gen1 = new ParticleSpringForceGenerator(particle[x][y], springConstant / 2, restLength * 2);
				ParticleFakeStiffSpringForceGenerator* gen1 = new ParticleFakeStiffSpringForceGenerator(particle[x][y], springConstant/2, damping);
				m_particleForceRegistry.Add(particle[x + 1][y], gen1);
				m_particleForceGenerators.push_back(gen1);

				//ParticleSpringForceGenerator* gen2 = new ParticleSpringForceGenerator(particle[x + 1][y], springConstant / 2, restLength * 2);
				ParticleFakeStiffSpringForceGenerator* gen2 = new ParticleFakeStiffSpringForceGenerator(particle[x+1][y], springConstant/2, damping);
				m_particleForceRegistry.Add(particle[x][y], gen2);
				m_particleForceGenerators.push_back(gen2);
			}
		}
	}


	// ---------------------------- LEVEL BOUND PLATFORMS CONTACTS ----------------------------
	{
		Vector3 levelBoundPlatformStartEnd[4][2] = {
			{ Vector3(-cameraLevelBounds.x, -cameraLevelBounds.y, 0), Vector3(cameraLevelBounds.x, -cameraLevelBounds.y, 0) },	// ground
			{ Vector3(cameraLevelBounds.x, cameraLevelBounds.y, 0), Vector3(-cameraLevelBounds.x, cameraLevelBounds.y, 0) },	// ceiling
			{ Vector3(-cameraLevelBounds.x, cameraLevelBounds.y, 0), Vector3(-cameraLevelBounds.x, -cameraLevelBounds.y, 0) },	// left wall
			{ Vector3(cameraLevelBounds.x, -cameraLevelBounds.y, 0), Vector3(cameraLevelBounds.x, cameraLevelBounds.y, 0) }	// right wall
		};
		for(int i = 0; i < 1; ++i)
		{
			Platform* platform = new Platform();
			platform->SetColorAndThickness(Colors::Blue, 10);
			platform->Initialize(levelBoundPlatformStartEnd[i][0], levelBoundPlatformStartEnd[i][1], m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
			m_platforms.push_back(platform);

			ParticlePlatformContactsGenerator* platformContactsGenerator = new ParticlePlatformContactsGenerator(levelBoundPlatformStartEnd[i][0], levelBoundPlatformStartEnd[i][1]);
			platformContactsGenerator->AddParticle(m_particleWorld->GetActiveParticles());
			m_particleContactGenerators.push_back(platformContactsGenerator);
			m_particleWorld->GetContactGenerators().push_back(platformContactsGenerator);
		}
	}

	// ---------------------------- FLYING PLATFORMS CONTACTS ----------------------------
	{
		Vector3 flyingPlatformStartEnd[2][2] = {
			{ Vector3(-100, -100, 0), Vector3(100, 0, 0) },	// left
			{ Vector3(100, 0, 0), Vector3(250, -50, 0) }		// right
		};
		for (int i = 0; i < 2; ++i)
		{
			Platform* platform = new Platform();
			platform->SetColorAndThickness(Colors::Blue, 10);
			platform->Initialize(flyingPlatformStartEnd[i][0], flyingPlatformStartEnd[i][1], m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
			m_platforms.push_back(platform);

			ParticlePlatformContactsGenerator* platformContactsGenerator = new ParticlePlatformContactsGenerator(flyingPlatformStartEnd[i][0], flyingPlatformStartEnd[i][1]);
			platformContactsGenerator->AddParticle(m_particleWorld->GetActiveParticles());
			m_particleContactGenerators.push_back(platformContactsGenerator);
			m_particleWorld->GetContactGenerators().push_back(platformContactsGenerator);
		}
	}

	// ---------------------------- SLOPE PLATFORM CONTACTS ----------------------------
	{
		Vector3 slopePlatformStartEnd[2] = { Vector3(-cameraLevelBounds.x, -cameraLevelBounds.y+300, 0.f), Vector3(-cameraLevelBounds.x + 200, -cameraLevelBounds.y, 0) };
		Platform* platform = new Platform();
		platform->SetColorAndThickness(Colors::Blue, 10);
		platform->Initialize(slopePlatformStartEnd[0], slopePlatformStartEnd[1], m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
		m_platforms.push_back(platform);

		ParticlePlatformContactsGenerator* platformContactsGenerator = new ParticlePlatformContactsGenerator(slopePlatformStartEnd[0], slopePlatformStartEnd[1]);
		platformContactsGenerator->AddParticle(m_particleWorld->GetActiveParticles());
		m_particleContactGenerators.push_back(platformContactsGenerator);
		m_particleWorld->GetContactGenerators().push_back(platformContactsGenerator);
	}

	// ---------------------------- PARTICLE VS PARTICLE CONTACTS ----------------------------
	{
		ParticleParticleContactGenerator* particleContactGenerator = new ParticleParticleContactGenerator();
		particleContactGenerator->AddParticle(m_particleWorld->GetActiveParticles());
		m_particleContactGenerators.push_back(particleContactGenerator);
		m_particleWorld->GetContactGenerators().push_back(particleContactGenerator);
	}


	// ---------------------------- BLIZZARD PARTICLE EMITTER ----------------------------
	{
		std::vector<ParticleManagement*> manageParticleIn;
		for(ParticleContactGenerator* contactGenerator : m_particleContactGenerators)
		{
			manageParticleIn.push_back(contactGenerator);
		}
		Vector2 quarterBounds = cameraLevelBounds / 2;
		//manageParticleIn.pop_back();
		m_blizzardParticleEmitter.push_back(new BlizzardParticleEmitter(m_particleWorld, manageParticleIn, m_snowGravity, Vector3(-cameraLevelBounds.x + quarterBounds.x, cameraLevelBounds.y - quarterBounds.y, 0), 1500));
		m_blizzardParticleEmitter.push_back(new BlizzardParticleEmitter(m_particleWorld, manageParticleIn, m_snowGravity, Vector3(cameraLevelBounds.x - quarterBounds.x, cameraLevelBounds.y - quarterBounds.y, 0), -1500));
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

	for(BlizzardParticleEmitter* blizzard : m_blizzardParticleEmitter)
	{
		blizzard->Update(elapsedTime);
	}

	m_particleWorld->RunPhysics(elapsedTime);

	m_camera.UpdateViewMatrix();
}

void Game::checkAndProcessKeyboardInput(const float& deltaTime)
{
	auto kb = m_keyboard->GetState();
	if (kb.Escape)
		PostQuitMessage(0);

	if(kb.IsKeyDown(Keyboard::Keys::F1))
	{
		m_particleWorld->DestroyAllSnow();
	}
	if (kb.IsKeyDown(Keyboard::Keys::F2))
	{
		m_particleWorld->DestroyAllBalls();
	}

	static bool qDown = false;
	if (kb.IsKeyDown(Keyboard::Keys::Q) && !qDown)
	{
		qDown = true;
		for (int i = 1; i <= 20; ++i)
		{
			Particle* particle = m_particleWorld->GetNewParticle();
			particle->SetPosition(Vector3(-330, 300, 0) + Vector3::UnitX * static_cast<float>(i)*30.f);
			particle->SetMass(10);
			particle->SetVelocity(Vector3::Down *(rand() % 300) + Vector3::Left *(rand() % 100) + Vector3::Right *(rand() % 100));
			particle->SetAcceleration(m_gravity);
			particle->SetWorldSpaceRadius(particle->GetMass());
			//particle->SetBouncinessFactor(fmodf(static_cast<float>(rand()) * 0.01f, 0.9f));
			particle->SetBouncinessFactor(0.2f);
			particle->SetType(ParticleTypes::Ball);
			for (ParticleContactGenerator* particleGenerator : m_particleContactGenerators)
			{
				particleGenerator->AddParticle(particle);
			}
		}
	}
	else if (kb.IsKeyUp(Keyboard::Keys::Q))
		qDown = false;

	static bool eDown = false;
	if (kb.IsKeyDown(Keyboard::Keys::E) /*&& !eDown*/)
	{
		eDown = true;

		for(auto particle : m_particleWorld->GetActiveParticles())
		{
			particle->SetAcceleration((particle->GetType() == ParticleTypes::Snow ? m_snowGravity : m_gravity) + m_fanAcceleration*m_fanAccelerationMultiplier);
		}
	}
	else if (kb.IsKeyUp(Keyboard::Keys::E)&& eDown)
	{
		for (auto particle : m_particleWorld->GetActiveParticles())
		{
			particle->SetAcceleration(particle->GetType() == ParticleTypes::Snow ? m_snowGravity : m_gravity);
		}
		eDown = false;
	}


	if (kb.IsKeyDown(Keyboard::Keys::Up))
	{
		++m_fanAccelerationMultiplier;
	}
	else if(kb.IsKeyDown(Keyboard::Keys::Down))
	{
		--m_fanAccelerationMultiplier;
	}
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
