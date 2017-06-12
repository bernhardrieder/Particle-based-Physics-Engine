#pragma once

class ParticleBungeeForceGenerator : public ParticleSpringForceGenerator
{
public:
	ParticleBungeeForceGenerator();
	ParticleBungeeForceGenerator(Particle* other, const float& springConstant, const float& restLength);

	void UpdateForce(Particle* particle, const float& deltaTime) override;
};

class ParticleAnchoredBungeeForceGenerator : public ParticleAnchoredSpringForceGenerator
{
public:
	ParticleAnchoredBungeeForceGenerator();
	ParticleAnchoredBungeeForceGenerator(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& restLength);

	void UpdateForce(Particle* particle, const float& deltaTime) override;
};