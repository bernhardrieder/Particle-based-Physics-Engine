#pragma once

class ParticleSpringForceGenerator : public ParticleForceGenerator
{
public:
	ParticleSpringForceGenerator();
	ParticleSpringForceGenerator(Particle* other, const float& springConstant, const float& restLength);

	virtual void UpdateForce(Particle* particle, const float& deltaTime) override;
	void Initialize(Particle* other, const float& springConstant, const float& restLength);

protected:
	Particle* m_other = nullptr;
	float m_springConstant = 1;
	float m_restLength = 1;
};

class ParticleAnchoredSpringForceGenerator : public ParticleForceGenerator
{
public:
	ParticleAnchoredSpringForceGenerator();
	ParticleAnchoredSpringForceGenerator(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& restLength);

	void Initalize(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& restLength);
	virtual void UpdateForce(Particle* particle, const float& deltaTime) override;
	const DirectX::SimpleMath::Vector3* GetAnchor() const;

protected:
	DirectX::SimpleMath::Vector3* m_anchor = nullptr;
	float m_springConstant = 1;
	float m_restLength = 1;
};

class ParticleFakeStiffSpringForceGenerator : public ParticleForceGenerator
{
public:
	ParticleFakeStiffSpringForceGenerator();
	ParticleFakeStiffSpringForceGenerator(Particle* other, const float& springConstant, const float& damping);

	void Initialize(Particle* other, const float& springConstant, const float& damping);
	void UpdateForce(Particle* particle, const float& deltaTime) override;

protected:
	Particle* m_other = nullptr;
	float m_springConstant = 1;
	float m_damping = 1;
};

class ParticleAnchoredFakeStiffSpringForceGenerator : public ParticleForceGenerator
{
public:
	ParticleAnchoredFakeStiffSpringForceGenerator();;
	ParticleAnchoredFakeStiffSpringForceGenerator(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& damping);

	void Initialize(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& damping);
	void UpdateForce(Particle* particle, const float& deltaTime) override;

protected:
	DirectX::SimpleMath::Vector3* m_anchor = nullptr;
	float m_springConstant = 1;
	float m_damping = 1;
};