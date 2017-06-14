#include "pch.h"
#include "BlizzardParticleEmitter.h"

using namespace DirectX::SimpleMath;

BlizzardParticleEmitter::BlizzardParticleEmitter(const std::vector<ParticleManagement*>& manageParticlesInThis, const DirectX::SimpleMath::Vector3& gravity, const DirectX::SimpleMath::Vector3& position, const float& rotationSpeed)
	: m_manageParticlesInThis(manageParticlesInThis), m_position(position), m_gravity(gravity), m_rotationSpeed(rotationSpeed)
{
}

BlizzardParticleEmitter::~BlizzardParticleEmitter()
{
}

void BlizzardParticleEmitter::Update(const float& deltaTime)
{
	Matrix rotationMatrix = Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_rotationSpeed*deltaTime));
	m_currentEmitDirection = Vector3::Transform(m_currentEmitDirection, rotationMatrix);

	emitParticle();
}
void BlizzardParticleEmitter::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

void BlizzardParticleEmitter::SetGravity(const DirectX::SimpleMath::Vector3& gravity)
{
	m_gravity = gravity;
}

void BlizzardParticleEmitter::SetRotationSpeed(const float rotationSpeed)
{
	m_rotationSpeed = rotationSpeed;
}

void BlizzardParticleEmitter::emitParticle()
{
	//todo: create pool
	Particle* particle = new Particle();
	particle->SetPosition(m_position);
	particle->SetMass(1);
	particle->SetVelocity(m_currentEmitDirection*100);
	particle->SetAcceleration(m_gravity);
	particle->SetWorldSpaceRadius(1);
	particle->SetBouncinessFactor(0.0f);
	m_particles.push_back(particle);

	for(auto& particleManager : m_manageParticlesInThis)
	{
		particleManager->AddParticle(particle);
	}
}
