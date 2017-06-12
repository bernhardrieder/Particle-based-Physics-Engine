#pragma once
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

	void SetMass(const float& mass);
	float GetMass() const;

	void AddForce(const DirectX::SimpleMath::Vector3& force);

	bool HasFiniteMass() const;

protected:
	void clearForceAccumulator();

	DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_velocity = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_acceleration = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_forceAccumulated = DirectX::SimpleMath::Vector3::Zero;

	// damping goes from 0 .. 1 -> veloctiy *= damping
	float m_damping = 0.99f;
	float m_mass = 0;
	float m_inverseMass = 0;
};

