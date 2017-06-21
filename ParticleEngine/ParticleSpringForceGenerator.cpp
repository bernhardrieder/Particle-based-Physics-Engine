#include "pch.h"
#include "ParticleSpringForceGenerator.h"

using namespace DirectX::SimpleMath;

ParticleSpringForceGenerator::ParticleSpringForceGenerator()
{
}

ParticleSpringForceGenerator::ParticleSpringForceGenerator(Particle* other, const float& springConstant, const float& restLength): m_other(other), m_springConstant(springConstant), m_restLength(restLength)
{
}

void ParticleSpringForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Calculate the vector of the spring
	Vector3 force = particle->GetPosition() - m_other->GetPosition();

	// Calculate the magnitude of the force
	float magnitude = force.Length();
	magnitude = fabsf(magnitude - m_restLength);
	magnitude *= m_springConstant;

	// Calculate the final force and apply it
	force.Normalize();
	force *= -magnitude;
	particle->AddForce(force);
}

void ParticleSpringForceGenerator::Initialize(Particle* other, const float& springConstant, const float& restLength)
{
	m_other = other;
	m_springConstant = springConstant;
	m_restLength = restLength;
}

ParticleAnchoredSpringForceGenerator::ParticleAnchoredSpringForceGenerator()
{
}

ParticleAnchoredSpringForceGenerator::ParticleAnchoredSpringForceGenerator(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& restLength)
	: m_anchor(anchor), m_springConstant(springConstant), m_restLength(restLength)
{
}

void ParticleAnchoredSpringForceGenerator::Initalize(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& restLength)
{
	m_anchor = anchor;
	m_springConstant = springConstant;
	m_restLength = restLength;
}

void ParticleAnchoredSpringForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Calculate the vector of the spring
	Vector3 force = particle->GetPosition() - *m_anchor;

	// Calculate the magnitude of the force
	float magnitude = force.Length();
	magnitude = (m_restLength - magnitude) * m_springConstant;

	// Calculate the final force and apply it
	force.Normalize();
	force *= magnitude;
	particle->AddForce(force);
}

const DirectX::SimpleMath::Vector3* ParticleAnchoredSpringForceGenerator::GetAnchor() const
{
	return m_anchor;
}

ParticleFakeStiffSpringForceGenerator::ParticleFakeStiffSpringForceGenerator()
{
}

ParticleFakeStiffSpringForceGenerator::ParticleFakeStiffSpringForceGenerator(Particle* other, const float& springConstant, const float& damping) : m_other(other), m_springConstant(springConstant), m_damping(damping)
{
}

void ParticleFakeStiffSpringForceGenerator::Initialize(Particle* other, const float& springConstant, const float& damping)
{
	m_other = other;
	m_springConstant = springConstant;
	m_damping = damping;
}

void ParticleFakeStiffSpringForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Check that we do not have infinite mass
	if (!particle->HasFiniteMass()) return;

	// Calculate the relative position of the particle to the anchor
	Vector3 position = particle->GetPosition() - m_other->GetPosition();

	// Calculate the constants and check they are in bounds.
	float gamma = 0.5f * sqrtf(4 * m_springConstant - m_damping*m_damping);
	if (gamma == 0.0f) return;

	Vector3 c = (position * (m_damping / (2.0f * gamma))) + (particle->GetVelocity() * (1.0f / gamma));

	// Calculate the target position
	Vector3 target = (position * cosf(gamma * deltaTime)) + (c * sinf(gamma * deltaTime));
	target *= expf(-0.5f * deltaTime * m_damping);

	// Calculate the resulting acceleration and therefore the force
	Vector3 accel = (target - position) * (1.0f / (deltaTime*deltaTime)) - particle->GetVelocity() * (1.0f / deltaTime);
	particle->AddForce(accel * particle->GetMass());
}

ParticleAnchoredFakeStiffSpringForceGenerator::ParticleAnchoredFakeStiffSpringForceGenerator()
{
}

ParticleAnchoredFakeStiffSpringForceGenerator::ParticleAnchoredFakeStiffSpringForceGenerator(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& damping): m_anchor(anchor), m_springConstant(springConstant), m_damping(damping)
{
}

void ParticleAnchoredFakeStiffSpringForceGenerator::Initialize(DirectX::SimpleMath::Vector3* anchor, const float& springConstant, const float& damping)
{
	m_anchor = anchor;
	m_springConstant = springConstant;
	m_damping = damping;
}

void ParticleAnchoredFakeStiffSpringForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
{
	// Check that we do not have infinite mass
	if (!particle->HasFiniteMass()) return;

	// Calculate the relative position of the particle to the anchor
	Vector3 position = particle->GetPosition() - *m_anchor;
	
	// Calculate the constants and check they are in bounds.
	float gamma = 0.5f * sqrtf(4 * m_springConstant - m_damping*m_damping);
	if (gamma == 0.0f) return;

	Vector3 c = (position * (m_damping / (2.0f * gamma))) + (particle->GetVelocity() * (1.0f / gamma));

	// Calculate the target position
	Vector3 target = (position * cosf(gamma * deltaTime)) + (c * sinf(gamma * deltaTime));
	target *= expf(-0.5f * deltaTime * m_damping);

	// Calculate the resulting acceleration and therefore the force
	Vector3 accel = (target - position) * (1.0f / (deltaTime*deltaTime)) - particle->GetVelocity() * (1.0f / deltaTime);
	particle->AddForce(accel * particle->GetMass());
}
