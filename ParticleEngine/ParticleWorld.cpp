#include "pch.h"
#include "ParticleWorld.h"

ParticleWorld::ParticleWorld(const int& maxContactsPerFrame, const int& contactResolutionIterations): m_contactResolver(contactResolutionIterations),
                                                                                                      m_maxContacts(maxContactsPerFrame)
{
	m_contacts = new ParticleContact[maxContactsPerFrame];
	m_shouldCalculateIterations = (contactResolutionIterations == 0);
}

ParticleWorld::~ParticleWorld()
{
	delete[] m_contacts;
	for(Particle* particle : m_particles)
	{
		delete particle;
	}
}

void ParticleWorld::StartFrame()
{
	for (Particle* particle : m_particles)
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
	for (Particle* particle : m_particles)
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
		unsigned used = contactGenerator->AddContact(nextContact, limitOfContacts);
		limitOfContacts -= used;
		nextContact += used;

		// We've run out of contacts to fill. This means we're missing
		// contacts.
		if (limitOfContacts <= 0) break;
	}

	// Return the number of contacts used.
	return m_maxContacts - limitOfContacts;
}

std::vector<Particle*>& ParticleWorld::GetParticles()
{
	return m_particles;
}

std::vector<ParticleContactGenerator*>& ParticleWorld::GetContactGenerators()
{
	return m_contactGenerators;
}

ParticleForceRegistry& ParticleWorld::GetForceRegistry()
{
	return m_registry;
}
