#include "pch.h"
#include "ParticleContactResolver.h"

using namespace DirectX::SimpleMath;

ParticleContactResolver::ParticleContactResolver(const unsigned& iterations)
	:
	m_iterations(iterations), m_iterationsUsed(0)
{
}

void ParticleContactResolver::SetIterations(const unsigned& iterations)
{
	m_iterations = iterations;
}

void ParticleContactResolver::ResolveContacts(ParticleContact *contactArray, const int& numContacts, const float& duration)
{
	int i;

	m_iterationsUsed = 0;
	while (m_iterationsUsed < m_iterations)
	{
		// Find the contact with the largest closing velocity;
		float max = std::numeric_limits<float>::max();
		int maxIndex = numContacts;
		for (i = 0; i < numContacts; i++)
		{
			float sepVel = contactArray[i].calculateSeparatingVelocity();
			if (sepVel < max &&
				(sepVel < 0 || contactArray[i].Penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		// Do we have anything worth resolving?
		if (maxIndex == numContacts) break;

		// resolve this contact
		contactArray[maxIndex].resolve(duration);

		// Update the interpenetrations for all particles
		Vector3 *move = contactArray[maxIndex].ParticleMovement;
		for (i = 0; i < numContacts; i++)
		{
			if (contactArray[i].ContactParticles[0] == contactArray[maxIndex].ContactParticles[0])
			{
				contactArray[i].Penetration -= move[0].Dot(contactArray[i].ContactNormal);
			}
			else if (contactArray[i].ContactParticles[0] == contactArray[maxIndex].ContactParticles[1])
			{
				contactArray[i].Penetration -= move[1].Dot(contactArray[i].ContactNormal);
			}
			if (contactArray[i].ContactParticles[1])
			{
				if (contactArray[i].ContactParticles[1] == contactArray[maxIndex].ContactParticles[0])
				{
					contactArray[i].Penetration += move[0].Dot(contactArray[i].ContactNormal);
				}
				else if (contactArray[i].ContactParticles[1] == contactArray[maxIndex].ContactParticles[1])
				{
					contactArray[i].Penetration += move[1].Dot(contactArray[i].ContactNormal);
				}
			}
		}

		m_iterationsUsed++;
	}
}
