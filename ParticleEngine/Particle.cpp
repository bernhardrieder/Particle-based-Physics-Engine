#include "pch.h"
#include "Particle.h"

using namespace DirectX::SimpleMath;

Particle::Particle()
{
}

Particle::~Particle()
{
}

void Particle::Integrate(const float& deltaTime)
{
	//don't integrate things with infite mass
	if (m_inverseMass <= 0.0f) return;
	assert(deltaTime > 0.0f);

	//update linear pos
	m_position += m_velocity * deltaTime;

	//work out acceleration from the force
	Vector3 resultingAcc = m_acceleration;
	resultingAcc += m_forceAccumulated * m_inverseMass;

	//update linear velocity from the acceleration
	m_velocity += resultingAcc * deltaTime;

	//impose drag
	m_velocity *= powf(m_damping, deltaTime);

	//clear the forces
	ClearForceAccumulator();
}

void Particle::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

DirectX::SimpleMath::Vector3 Particle::GetPosition() const
{
	return m_position;
}

void Particle::SetVelocity(const DirectX::SimpleMath::Vector3& velocity)
{
	m_velocity = velocity;
}

DirectX::SimpleMath::Vector3 Particle::GetVelocity() const
{
	return m_velocity;
}

void Particle::SetAcceleration(const DirectX::SimpleMath::Vector3& acceleration)
{
	m_acceleration = acceleration;
}

DirectX::SimpleMath::Vector3 Particle::GetAcceleration() const
{
	return m_acceleration;
}

void Particle::SetMass(const float& mass)
{
	m_mass = mass;
	m_inverseMass = 1.f / mass;
}

float Particle::GetMass() const
{
	return m_mass;
}

float Particle::GetInverseMass() const
{
	return m_inverseMass;
}

void Particle::SetBouncinessFactor(const float& bouncinessFactor)
{
	m_bouncinessFactor = bouncinessFactor;
}

bool Particle::HasFiniteMass() const
{
	return m_inverseMass >= 0.0f;
}

void Particle::ClearForceAccumulator()
{
	m_forceAccumulated = Vector3::Zero;
}

void Particle::SetWorldSpaceRadius(const float& radius)
{
	m_worldSpaceRadius = radius;
}

float Particle::GetWorldSpaceRadius() const
{
	return m_worldSpaceRadius;
}

void Particle::SetActive(bool active)
{
	m_isActive = active;
}

bool Particle::IsActive() const
{
	return m_isActive;
}

void Particle::SetType(ParticleTypes type)
{
	m_type = type;
}

ParticleTypes Particle::GetType() const
{
	return m_type;
}

float Particle::GetBouncinessFactor() const
{
	return m_bouncinessFactor;
}

void Particle::AddForce(const DirectX::SimpleMath::Vector3& force)
{
	m_forceAccumulated += force;
}

void ParticleManagement::AddParticle(Particle* particle)
{
	m_particles.push_back(particle);
}

void ParticleManagement::AddParticle(const std::vector<Particle*>& particles)
{
	for(Particle* particle : particles)
	{
		AddParticle(particle);
	}
}

void ParticleManagement::RemoveParticle(Particle* particle)
{
	for (size_t index = 0; index < m_particles.size(); ++index)
	{
		if (m_particles[index] == particle)
		{
			m_particles.erase(m_particles.begin() + index);
			break;
		}
	}
}

std::vector<Particle*>& ParticleManagement::GetParticles()
{
	return m_particles;
}