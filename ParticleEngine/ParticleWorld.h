#pragma once
#include "ParticleContacts.h"
#include "ParticleContactGenerators.h"

class ParticleWorld : public ParticleManagement
{
public:
	ParticleWorld(const int& maxContactsPerFrame, const int& contactResolutionIterations = 0);
	~ParticleWorld();

	void StartFrame();
	void RunPhysics(const float& deltaTime);
	void IntegrateAllParticles(const float& deltaTime);
	int GenerateContactsWithRegisteredContactGeneratorsAndReturnNumOfContacts();

	std::vector<Particle*>& GetParticles();
	std::vector<ParticleContactGenerator*>& GetContactGenerators();
	ParticleForceRegistry& GetForceRegistry();

protected:
	bool m_shouldCalculateIterations = false;
	ParticleForceRegistry m_registry;
	ParticleContactResolver m_contactResolver;
	std::vector<ParticleContactGenerator*> m_contactGenerators;
	ParticleContact* m_contacts = nullptr;
	int m_maxContacts = 0;
};

