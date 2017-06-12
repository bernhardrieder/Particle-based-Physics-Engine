#pragma once

class Particle;

class ParticleForceGenerator
{
public:
	virtual ~ParticleForceGenerator() = default;
	virtual void UpdateForce(Particle* particle, const float& deltaTime) = 0;
};