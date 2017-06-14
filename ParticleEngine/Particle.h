#pragma once

enum class ParticleTypes : int
{
	None,
	Ball,
	Snow,
	Cloth
};

class Particle
{
public:
	Particle();
	~Particle();

	void Integrate(const float& deltaTime);

	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	DirectX::SimpleMath::Vector3 GetPosition() const;

	void SetVelocity(const DirectX::SimpleMath::Vector3& velocity);
	DirectX::SimpleMath::Vector3 GetVelocity() const;

	void SetAcceleration(const DirectX::SimpleMath::Vector3& acceleration);
	DirectX::SimpleMath::Vector3 GetAcceleration() const;

	void SetMass(const float& mass);
	float GetMass() const;
	float GetInverseMass() const;

	void SetBouncinessFactor(const float& bouncinessFactor);
	float GetBouncinessFactor() const;

	void AddForce(const DirectX::SimpleMath::Vector3& force);

	bool HasFiniteMass() const;
	void ClearForceAccumulator();

	void SetWorldSpaceRadius(const float& radius);
	float GetWorldSpaceRadius() const;

	void SetActive(bool active);
	bool IsActive() const;

	void SetType(ParticleTypes type);
	ParticleTypes GetType() const;

protected:
	DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_velocity = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_acceleration = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_forceAccumulated = DirectX::SimpleMath::Vector3::Zero;

	// damping goes from 0 .. 1 -> veloctiy *= damping
	float m_damping = 0.99f;
	float m_mass = 0;
	float m_inverseMass = 0;
	float m_worldSpaceRadius = 1;
	float m_bouncinessFactor = 0.0f;
	bool m_isActive = false;
	ParticleTypes m_type;
};

class ParticleManagement
{
public:
	void AddParticle(Particle* particle);
	void AddParticle(const std::vector<Particle*>& particles);
	void RemoveParticle(Particle* particle);
	std::vector<Particle*>& GetParticles();

protected:
	std::vector<Particle*> m_particles;
};
