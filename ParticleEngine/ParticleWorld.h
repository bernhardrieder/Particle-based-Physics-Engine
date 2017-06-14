#pragma once
#include "ParticleContact.h"
#include "ParticleContactGenerators.h"
#include "ParticleContactResolver.h"

class ParticleWorld
{
public:
	ParticleWorld(const int& maxContactsPerFrame, const int& poolSize, const DirectX::SimpleMath::Vector2& levelBounds, const int& contactResolutionIterations = 0);
	~ParticleWorld();

	void StartFrame();
	void RunPhysics(const float& deltaTime);
	void IntegrateAllParticles(const float& deltaTime);
	int GenerateContactsWithRegisteredContactGeneratorsAndReturnNumOfContacts();

	std::vector<Particle*>& GetActiveParticles();
	std::vector<ParticleContactGenerator*>& GetContactGenerators();
	ParticleForceRegistry& GetForceRegistry();

	Particle* GetNewParticle();
	void ReleaseParticle(Particle* particle);

	void DestroyAllSnow();
	void DestroyAllBalls();

protected:
	void createPool(const int& poolSize);
	static void removeInactiveParticles(std::vector<Particle*>& particles);
	void releaseInactiveParticles();
	void disableActiveParticleOutOfLevelBounds();
	void destroy(ParticleTypes type);

	std::vector<Particle*> m_particlePool;
	std::vector<Particle*> m_activeParticles;
	bool m_shouldCalculateIterations = false;
	ParticleForceRegistry m_registry;
	ParticleContactResolver m_contactResolver;
	std::vector<ParticleContactGenerator*> m_contactGenerators;
	ParticleContact* m_contacts = nullptr;
	int m_maxContacts = 0;
	DirectX::SimpleMath::Vector2 m_levelBounds;
};

