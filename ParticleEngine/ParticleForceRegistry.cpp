#include "pch.h"
#include "ParticleForceRegistry.h"


ParticleForceRegistry::ParticleForceRegistry()
{
}


ParticleForceRegistry::~ParticleForceRegistry()
{
}

void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* forceGenerator)
{
	ParticleForceRegistration registration;
	registration.Particle = particle;
	registration.ForceGenerator = forceGenerator;
	m_registrations.push_back(registration);
}

void ParticleForceRegistry::Remove(Particle* particle, ParticleForceGenerator* forceGenerator)
{
	for (size_t index = 0; index < m_registrations.size(); ++index)
	{
		if (m_registrations[index].Particle == particle && m_registrations[index].ForceGenerator == forceGenerator)
		{
			m_registrations.erase(m_registrations.begin() + index);
			break;
		}
	}
}

void ParticleForceRegistry::Clear()
{
	m_registrations.clear();
}

void ParticleForceRegistry::UpdateForces(const float& deltaTime)
{
	removeInactiveParticle();
	for (ParticleForceRegistration& i : m_registrations)
	{
		i.ForceGenerator->UpdateForce(i.Particle, deltaTime);
	}
}

void ParticleForceRegistry::removeInactiveParticle()
{
	for (std::vector<ParticleForceRegistration>::iterator it = m_registrations.begin(); it != m_registrations.end();)
	{
		if (!(*it).Particle->IsActive())
			it = m_registrations.erase(it);
		else
			++it;
	}
}
