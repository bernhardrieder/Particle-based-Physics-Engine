#pragma once
class Particle;

class ParticleForceGenerator
{
public:
	virtual ~ParticleForceGenerator() = default;
	virtual void UpdateForce(Particle* particle, const float& deltaTime) = 0;
};

class ParticleGravity : public ParticleForceGenerator
{
public:
	ParticleGravity();
	ParticleGravity(const DirectX::SimpleMath::Vector3 &gravity);
	void UpdateForce(Particle* particle, const float& deltaTime) override;
	void SetGravity(const DirectX::SimpleMath::Vector3& gravity);

private:
	DirectX::SimpleMath::Vector3 m_gravity;
};

class ParticleDrag : public ParticleForceGenerator
{
public:
	ParticleDrag(float velocityDrag);
	void UpdateForce(Particle* particle, const float& deltaTime) override;
	void SetDragCoefficients(float velocityDrag);

private:
	float m_velocityDrag;
	float m_velocityDragSquared;
};

