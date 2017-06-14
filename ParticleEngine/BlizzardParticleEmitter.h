#pragma once
class BlizzardParticleEmitter : ParticleManagement
{
public:
	BlizzardParticleEmitter() = delete;
	BlizzardParticleEmitter(const std::vector<ParticleManagement*>& manageParticlesInThis, const DirectX::SimpleMath::Vector3& gravity, const DirectX::SimpleMath::Vector3& position, const float& rotationSpeed);
	~BlizzardParticleEmitter();
	
	void Update(const float& deltaTime);

	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	void SetGravity(const DirectX::SimpleMath::Vector3& gravity);
	void SetRotationSpeed(const float rotationSpeed);

private:
	void emitParticle();

	std::vector<ParticleManagement*> m_manageParticlesInThis;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_gravity;
	DirectX::SimpleMath::Vector3 m_currentEmitDirection = DirectX::SimpleMath::Vector3::Up;
	float m_rotationSpeed = 10;
};

