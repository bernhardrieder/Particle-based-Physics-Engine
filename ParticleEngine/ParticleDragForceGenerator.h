#pragma once

class ParticleDragForceGenerator : public ParticleForceGenerator
{
public:
	ParticleDragForceGenerator();
	ParticleDragForceGenerator(const float& velocityDrag);

	void UpdateForce(Particle* particle, const float& deltaTime) override;
	void SetDragCoefficients(const float& velocityDrag);

private:
	float m_velocityDrag = 1;
	float m_velocityDragSquared = 1;
};