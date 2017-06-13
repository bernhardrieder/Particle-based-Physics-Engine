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

ParticlePlatformContactsGenerator::ParticlePlatformContactsGenerator()
{
}

ParticlePlatformContactsGenerator::ParticlePlatformContactsGenerator(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end): m_start(start), m_end(end)
{
}

void ParticlePlatformContactsGenerator::Initialize(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end)
{
	m_start = start;
	m_end = end;
}

int ParticlePlatformContactsGenerator::AddContact(ParticleContact* contact, const int& limit) const
{
	float restitution = 0.0f;

	int used = 0;
	for (Particle* particle : m_particles)
	{
		if (used >= limit) break;

		// Check for penetration
		Vector3 toParticle = particle->GetPosition() - m_start;
		Vector3 lineDirection = m_end - m_start;
		float projected = toParticle.Dot(lineDirection);
		float platformSqLength = lineDirection.LengthSquared();
		if (projected <= 0)
		{
			Vector3 toParticleNormalized = toParticle;
			toParticleNormalized.Normalize();
			// The blob is nearest to the start point
			if (toParticle.LengthSquared() < std::powf(particle->GetWorldSpaceRadius(),2.f))
			{
				// We have a collision
				contact->ContactNormal = toParticleNormalized;
				contact->ContactNormal.z = 0;
				contact->Restitution = restitution;
				contact->ContactParticles[0] = particle;
				contact->ContactParticles[1] = nullptr;
				contact->Penetration = particle->GetWorldSpaceRadius() - toParticle.Length();
				used++;
				contact++;
			}

		}
		else if (projected >= platformSqLength)
		{
			// The blob is nearest to the end point
			toParticle = particle->GetPosition() - m_end;
			Vector3 toParticleNormalized = toParticle;
			toParticleNormalized.Normalize();
			if (toParticle.LengthSquared() < std::powf(particle->GetWorldSpaceRadius(), 2.f))
			{
				// We have a collision
				contact->ContactNormal = toParticleNormalized;
				contact->ContactNormal.z = 0;
				contact->Restitution = restitution;
				contact->ContactParticles[0] = particle;
				contact->ContactParticles[1] = nullptr;
				contact->Penetration = particle->GetWorldSpaceRadius() - toParticle.Length();
				used++;
				contact++;
			}
		}
		else
		{
			// the blob is nearest to the middle.
			float distanceToPlatform = toParticle.LengthSquared() - projected*projected / platformSqLength;
			if (distanceToPlatform < std::powf(particle->GetWorldSpaceRadius(), 2.f))
			{
				// We have a collision
				Vector3 closestPoint =	m_start + lineDirection*(projected / platformSqLength);
				Vector3 contactNormal = (particle->GetPosition() - closestPoint);
				contactNormal.Normalize();

				contact->ContactNormal = contactNormal;
				contact->ContactNormal.z = 0;
				contact->Restitution = restitution;
				contact->ContactParticles[0] = particle;
				contact->ContactParticles[1] = nullptr;
				contact->Penetration = particle->GetWorldSpaceRadius() - sqrtf(distanceToPlatform);
				used++;
				contact++;
			}
		}
	}
	return used;
}
