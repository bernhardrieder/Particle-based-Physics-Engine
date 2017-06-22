#include "pch.h"
#include "ParticleGravityForceGenerator.h"

using namespace DirectX::SimpleMath;

ParticleGravityForceGenerator::ParticleGravityForceGenerator() : ParticleGravityForceGenerator(Vector3::Down * 10)
{
}

ParticleGravityForceGenerator::ParticleGravityForceGenerator(const DirectX::SimpleMath::Vector3& gravity)
	: m_gravity(gravity)
{
}

void ParticleGravityForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Check that we do not have infinite mass
	if (!particle->HasFiniteMass()) return;

	// Apply the mass-scaled force to the particle
	particle->AddForce(m_gravity * particle->GetMass());
}

void ParticleGravityForceGenerator::SetGravity(const DirectX::SimpleMath::Vector3& gravity)
{
	m_gravity = gravity;
}

