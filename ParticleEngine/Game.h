//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game();
	~Game();

	// Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	void checkAndProcessKeyboardInput(const float& deltaTime);
	void checkAndProcessMouseInput(const float& deltaTime);

	void createClothParticle();
	void createLevelBoundPlatformsAndContactGenerator(DirectX::SimpleMath::Vector2 cameraLevelBounds);
	void createSlopePlatformAndContactGenerator(DirectX::SimpleMath::Vector2 cameraLevelBounds);
	void createFlyingPlatformsAndContactGenerator();
	void createParticleVsParticleContactGenerator();
	void createBlizzardParticleEmitter(DirectX::SimpleMath::Vector2 cameraLevelBounds);


    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;


	Camera m_camera;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	std::vector<ParticleForceGenerator*> m_particleForceGenerators;
	std::vector<ParticleContactGenerator*> m_particleContactGenerators;
	ParticleRenderer* m_particleRenderer = nullptr;
	ParticleWorld* m_particleWorld;
	std::vector<Platform*> m_platforms;
	std::vector<BlizzardParticleEmitter*> m_blizzardParticleEmitter;
	DirectX::SimpleMath::Vector3 m_gravity = DirectX::SimpleMath::Vector3::Down * 100;
	DirectX::SimpleMath::Vector3 m_snowGravity = DirectX::SimpleMath::Vector3::Down * 5;
	DirectX::SimpleMath::Vector3 m_fanAcceleration = DirectX::SimpleMath::Vector3::Left * 100;
	int m_fanAccelerationMultiplier = 1;
	DirectX::SimpleMath::Vector3 m_particleAnchor[3];

};