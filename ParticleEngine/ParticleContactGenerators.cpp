#include "pch.h"
#include "ParticleContactGenerators.h"

using namespace DirectX::SimpleMath;

int ParticleGroundContactsGenerator::AddContact(ParticleContact* contact, const int& limit) const
{
	int count = 0;
	for (Particle* particle : m_particles)
	{
		float y = particle->GetPosition().y;
		if (y < 0.0f)
		{
			contact->ContactNormal = Vector3::Up;
			contact->ContactParticles[0] = particle;
			contact->ContactParticles[1] = nullptr;
			contact->Penetration = -y;
			contact->Restitution = 0.2f;
			contact++;
			count++;
		}

		if (count >= limit) return count;
	}
	return count;
}
