#pragma once

class ParticleGravityForceGenerator : public ParticleForceGenerator
{
public:
	ParticleGravityForceGenerator();
	ParticleGravityForceGenerator(const DirectX::SimpleMath::Vector3 &gravity);

	void UpdateForce(Particle* particle, const float& deltaTime) override;
	void SetGravity(const DirectX::SimpleMath::Vector3& gravity);

private:
	DirectX::SimpleMath::Vector3 m_gravity = DirectX::SimpleMath::Vector3(0, -10, 0);
};
