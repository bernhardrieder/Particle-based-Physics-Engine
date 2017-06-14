#include "pch.h"
#include "BlizzardParticleEmitter.h"

using namespace DirectX::SimpleMath;

BlizzardParticleEmitter::BlizzardParticleEmitter(ParticleWorld* particleWorld, const std::vector<ParticleManagement*>& manageParticlesInThis, const DirectX::SimpleMath::Vector3& gravity, const DirectX::SimpleMath::Vector3& position, const float& rotationSpeed)
	: m_particleWorld(particleWorld), m_manageParticlesInThis(manageParticlesInThis), m_position(position), m_gravity(gravity), m_rotationSpeed(rotationSpeed)
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
	Particle* particle = m_particleWorld->GetNewParticle();
	if (!particle)
		return;
	particle->SetPosition(m_position);
	particle->SetMass(2);
	particle->SetVelocity(m_currentEmitDirection*50);
	particle->SetAcceleration(m_gravity);
	particle->SetWorldSpaceRadius(particle->GetMass());
	particle->SetBouncinessFactor(0.0f);

	for(auto& particleManager : m_manageParticlesInThis)
	{
		particleManager->AddParticle(particle);
	}
}
