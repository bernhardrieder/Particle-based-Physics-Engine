#include "pch.h"
#include "ParticleForceGenerator.h"

using namespace DirectX::SimpleMath;

ParticleGravity::ParticleGravity() : ParticleGravity(Vector3::Down*10)
{
}

ParticleGravity::ParticleGravity(const DirectX::SimpleMath::Vector3& gravity) : m_gravity(gravity)
{
}

void ParticleGravity::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Check that we do not have infinite mass
	if (!particle->HasFiniteMass()) return;

	// Apply the mass-scaled force to the particle
	particle->AddForce(m_gravity * particle->GetMass());
}

void ParticleGravity::SetGravity(const DirectX::SimpleMath::Vector3& gravity)
{
	m_gravity = gravity;
}

ParticleDrag::ParticleDrag(float velocityDrag) : m_velocityDrag(velocityDrag), m_velocityDragSquared(velocityDrag*velocityDrag)
{
}

void ParticleDrag::UpdateForce(Particle* particle, const float& deltaTime)
{
	Vector3 force = particle->GetVelocity();

	// Calculate the total drag coefficient
	float dragCoeff = force.Length();
	dragCoeff = m_velocityDrag * dragCoeff + m_velocityDragSquared * dragCoeff * dragCoeff;

	// Calculate the final force and apply it
	force.Normalize();
	force *= -dragCoeff;
	particle->AddForce(force);
}

void ParticleDrag::SetDragCoefficients(float velocityDrag)
{
	m_velocityDrag = velocityDrag;
}
