#include "pch.h"
#include "ParticleBungeeForceGenerator.h"

using namespace DirectX::SimpleMath;

ParticleBungeeForceGenerator::ParticleBungeeForceGenerator(): ParticleSpringForceGenerator()
{
}

ParticleBungeeForceGenerator::ParticleBungeeForceGenerator(Particle* other, const float& springConstant, const float& restLength): ParticleSpringForceGenerator(other, springConstant, restLength)
{
}

void ParticleBungeeForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{    
	// Calculate the vector of the spring
	//Vector3 force = particle->GetPosition() - m_other->GetPosition();
	Vector3 force = m_other->GetPosition() - particle->GetPosition();

	// Check if the bungee is compressed
	float magnitude = force.Length();
	if (magnitude <= m_restLength) return;

	// Calculate the magnitude of the force
	magnitude = m_springConstant * (m_restLength - magnitude);

	// Calculate the final force and apply it
	force.Normalize();
	force *= -magnitude;
	particle->AddForce(force);
}

ParticleAnchoredBungeeForceGenerator::ParticleAnchoredBungeeForceGenerator(): ParticleAnchoredSpringForceGenerator()
{
}

ParticleAnchoredBungeeForceGenerator::ParticleAnchoredBungeeForceGenerator(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& restLength): ParticleAnchoredSpringForceGenerator(anchor, springConstant, restLength)
{
}

void ParticleAnchoredBungeeForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Calculate the vector of the spring
	Vector3 force = particle->GetPosition() - *m_anchor;

	// Calculate the magnitude of the force
	float magnitude = force.Length();
	if (magnitude < m_restLength) return;

	magnitude = magnitude - m_restLength;
	magnitude *= m_springConstant;

	// Calculate the final force and apply it
	force.Normalize();
	force *= -magnitude;
	particle->AddForce(force);
}
