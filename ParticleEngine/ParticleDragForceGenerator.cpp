#include "pch.h"
#include "ParticleDragForceGenerator.h"

using namespace DirectX::SimpleMath;

ParticleDragForceGenerator::ParticleDragForceGenerator()
{
}

ParticleDragForceGenerator::ParticleDragForceGenerator(const float& velocityDrag)
	: m_velocityDrag(velocityDrag), m_velocityDragSquared(velocityDrag*velocityDrag)
{
}

void ParticleDragForceGenerator::UpdateForce(Particle* particle, const float& deltaTime)
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

void ParticleDragForceGenerator::SetDragCoefficients(const float& velocityDrag)
{
	m_velocityDrag = velocityDrag;
}

