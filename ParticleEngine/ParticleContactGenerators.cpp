#include "pch.h"
#include "ParticleContactGenerators.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

int ParticleGroundContactsGenerator::AddContact(ParticleContact* contact, const int& limit)
{
	int count = 0;
	for (Particle* particle : m_particles)
	{
		float y = particle->GetPosition().y;
		if (y < m_ground)
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

int ParticlePlatformContactsGenerator::AddContact(ParticleContact* contact, const int& limit) 
{
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
				contact->Restitution = particle->GetBouncinessFactor();
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
				contact->Restitution = particle->GetBouncinessFactor();
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
				contact->Restitution = particle->GetBouncinessFactor();
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

ParticleParticleContactGenerator::ParticleParticleContactGenerator(): ParticleContactGenerator()
{
	m_usedParticles.resize(std::numeric_limits<short>::max());
}

int ParticleParticleContactGenerator::AddContact(ParticleContact* contact, const int& limit) 
{
	m_usedParticleIndex = 0;
	int count = 0;
	for (Particle* particle : m_particles)
	{
		for (Particle* other : m_particles)
		{
			if (particle == other ||(particle->GetType() == ParticleTypes::Snow && other->GetType() == ParticleTypes::Snow))
				continue;

			Vector3 posParticle = particle->GetPosition();
			Vector3 posOther = other->GetPosition();

			Vector3 midline = posParticle - posOther;
			float size = midline.Length();

			if (size <= 0.0f || size >= particle->GetWorldSpaceRadius() + other->GetWorldSpaceRadius())
				continue;

			if (particlePairUsed(particle, other))
				continue;

			bool destroyParticle, destroyOther;
			shouldBeDestroyed(particle, other, destroyParticle, destroyOther);
			if(destroyOther || destroyParticle)
			{
				particle->SetActive(!destroyParticle);
				other->SetActive(!destroyOther);
				continue;
			}
			
			Vector3 normal = midline * (1.f / size);
			normal.Normalize();

			contact->ContactNormal = normal;
			contact->ContactParticles[0] = particle;
			contact->ContactParticles[1] = other;
			contact->Penetration = particle->GetWorldSpaceRadius() + other->GetWorldSpaceRadius() - size;
			contact->Restitution = particle->GetBouncinessFactor() + other->GetBouncinessFactor();
			contact++;
			count++;

			//todo: should create a branch for out of range
			m_usedParticles[m_usedParticleIndex].first = particle;
			m_usedParticles[m_usedParticleIndex].second = other;
			++m_usedParticleIndex;

			if (count >= limit)
				return count;
		}
	}
	return count;
}

bool ParticleParticleContactGenerator::particlePairUsed(Particle* one, Particle* two) const
{
	for (int i = 0; i < m_usedParticleIndex; ++i)
	{
		if (m_usedParticles[i].first == one && m_usedParticles[i].second == two ||
			m_usedParticles[i].first == two && m_usedParticles[i].second == one)
			return true;
	}
	return false;
}

void ParticleParticleContactGenerator::shouldBeDestroyed(Particle* lhs, Particle* rhs, bool& outDestroyLhs, bool& outDestroyRhs)
{
	//todo: try to convert this into a non hardcoded style!
	outDestroyLhs = false;
	outDestroyRhs = false;
	
	//BALL VS SNOW
	if (lhs->GetType() == ParticleTypes::Ball && rhs->GetType() == ParticleTypes::Snow)
	{
		outDestroyRhs = true;
		return;
	}
	if (rhs->GetType() == ParticleTypes::Ball && lhs->GetType() == ParticleTypes::Snow)
	{
		outDestroyLhs = true;
		return;
	}
	
	//CLOTH VS SNOW
	if (lhs->GetType() == ParticleTypes::Cloth && rhs->GetType() == ParticleTypes::Snow)
	{
		outDestroyRhs = true;
		return;
	}
	if (rhs->GetType() == ParticleTypes::Cloth && lhs->GetType() == ParticleTypes::Snow)
	{
		outDestroyLhs = true;
		return;
	}
}
