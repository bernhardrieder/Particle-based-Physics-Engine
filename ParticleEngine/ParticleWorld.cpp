#include "pch.h"
#include "ParticleWorld.h"

using namespace DirectX::SimpleMath;

ParticleWorld::ParticleWorld(const int& maxContactsPerFrame, const int& poolSize, const DirectX::SimpleMath::Vector2& levelBounds, const int& contactResolutionIterations) 
: m_contactResolver(contactResolutionIterations), m_maxContacts(maxContactsPerFrame), m_levelBounds(levelBounds)
{
	m_contacts = new ParticleContact[maxContactsPerFrame];
	m_shouldCalculateIterations = (contactResolutionIterations == 0);
	createPool(poolSize);
}

ParticleWorld::~ParticleWorld()
{
	delete[] m_contacts;
	for(Particle* particle : m_activeParticles)
	{
		delete particle;
	}
	for (Particle* particle : m_particlePool)
	{
		delete particle;
	}
}

void ParticleWorld::StartFrame()
{
	disableActiveParticleOutOfLevelBounds();
	releaseInactiveParticles();
	for (Particle* particle : m_activeParticles)
	{
		particle->ClearForceAccumulator();
	}
}

void ParticleWorld::RunPhysics(const float& deltaTime)
{
	// First apply the force generators
	m_registry.UpdateForces(deltaTime);

	// Then integrate the objects
	IntegrateAllParticles(deltaTime);

	// Generate contacts
	int usedContacts = GenerateContactsWithRegisteredContactGeneratorsAndReturnNumOfContacts();

	// And process them
	if (usedContacts)
	{
		if (m_shouldCalculateIterations)
		{
			m_contactResolver.SetIterations(usedContacts * 2);
		}
		m_contactResolver.ResolveContacts(m_contacts, usedContacts, deltaTime);
	}
}

void ParticleWorld::IntegrateAllParticles(const float& deltaTime)
{
	for (Particle* particle : m_activeParticles)
	{
		particle->Integrate(deltaTime);
	}
}

int ParticleWorld::GenerateContactsWithRegisteredContactGeneratorsAndReturnNumOfContacts()
{
	int limitOfContacts = m_maxContacts;
	ParticleContact* nextContact = m_contacts;

	for (ParticleContactGenerator* contactGenerator : m_contactGenerators)
	{
		removeInactiveParticles(contactGenerator->GetParticles());
		int used = contactGenerator->AddContact(nextContact, limitOfContacts);
		limitOfContacts -= used;
		nextContact += used;

		// We've run out of contacts to fill. This means we're missing
		// contacts.
		if (limitOfContacts <= 0) break;
	}

	// Return the number of contacts used.
	return m_maxContacts - limitOfContacts;
}

std::vector<Particle*>& ParticleWorld::GetActiveParticles()
{
	return m_activeParticles;
}

std::vector<ParticleContactGenerator*>& ParticleWorld::GetContactGenerators()
{
	return m_contactGenerators;
}

ParticleForceRegistry& ParticleWorld::GetForceRegistry()
{
	return m_registry;
}

Particle* ParticleWorld::GetNewParticle()
{
	if (m_particlePool.size() <= 0)
		return nullptr;

	Particle* particle = m_particlePool.back();
	m_particlePool.pop_back();
	particle->SetActive(true);
	particle->SetType(ParticleTypes::None);
	m_activeParticles.push_back(particle);
	return particle;
}

void ParticleWorld::ReleaseParticle(Particle* particle)
{
	particle->SetActive(false);
	removeInactiveParticles(m_activeParticles);
	m_particlePool.push_back(particle);
}

void ParticleWorld::createPool(const int& poolSize)
{
	for(int i = 0; i < poolSize; ++i)
	{
		m_particlePool.push_back(new Particle);
	}
}

void ParticleWorld::removeInactiveParticles(std::vector<Particle*>& particles)
{
	for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end();) 
	{
		if (!(*it)->IsActive())
			it = particles.erase(it);
		else
			++it;
	}
}

void ParticleWorld::releaseInactiveParticles()
{

	for (Particle* particle : m_activeParticles)
	{
		if (!particle->IsActive())	
			m_particlePool.push_back(particle);
	}
	removeInactiveParticles(m_activeParticles);
}

void ParticleWorld::disableActiveParticleOutOfLevelBounds()
{
	for (Particle* particle : m_activeParticles)
	{
		Vector3 pos = particle->GetPosition();
		if(/*pos.x < -m_levelBounds.x || pos.x > m_levelBounds.x ||*/ 
			pos.y < -m_levelBounds.y /*|| pos.y > m_levelBounds.y*/)
		{
			particle->SetActive(false);
		}
	}
}

void ParticleWorld::destroy(ParticleTypes type)
{
	for (Particle* particle : m_activeParticles)
	{
		if (particle->GetType() == type)
			particle->SetActive(false);
	}
	releaseInactiveParticles();
}

void ParticleWorld::DestroyAllSnow()
{
	destroy(ParticleTypes::Snow);
}

void ParticleWorld::DestroyAllBalls()
{
	destroy(ParticleTypes::Ball);
}
