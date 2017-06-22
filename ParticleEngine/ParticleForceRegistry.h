#pragma once
class ParticleForceGenerator;
class Particle;

class ParticleForceRegistry
{
public:
	ParticleForceRegistry();
	~ParticleForceRegistry();

	void Add(Particle* particle, ParticleForceGenerator* forceGenerator);
	void Remove(Particle* particle, ParticleForceGenerator* forceGenerator);
	void Clear();
	void UpdateForces(const float& deltaTime);

protected:
	void removeInactiveParticle();

	struct ParticleForceRegistration
	{
		Particle* Particle;
		ParticleForceGenerator* ForceGenerator;
	};

	std::vector<ParticleForceRegistration> m_registrations;
};
