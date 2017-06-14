#pragma once

/**
* This is the basic polymorphic interface for contact generators
* applying to particles.
*/
class ParticleContactGenerator : public ParticleManagement
{
public:
	virtual ~ParticleContactGenerator() = default;
	/**
	* Fills the given contact structure with the generated
	* contact. The contact pointer should point to the first
	* available contact in a contact array, where limit is the
	* maximum number of contacts in the array that can be written
	* to. The method returns the number of contacts that have
	* been written.
	*/
	virtual int AddContact(ParticleContact* contact, const int& limit) const = 0;
};

/**
* A contact generator that takes an STL vector of particle pointers and
* collides them against the ground.
*/
class ParticleGroundContactsGenerator : public ParticleContactGenerator
{
public:
	int AddContact(ParticleContact* contact, const int& limit) const override;
};

/**
* Platforms are two dimensional: lines on which the
* particles can rest. Platforms are also contact generators for the physics.
*/
class ParticlePlatformContactsGenerator : public ParticleContactGenerator
{
public:
	ParticlePlatformContactsGenerator();
	ParticlePlatformContactsGenerator(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end);

	void Initialize(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end);
	int AddContact(ParticleContact* contact, const int& limit) const override;

private:
	DirectX::SimpleMath::Vector3 m_start = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 m_end = DirectX::SimpleMath::Vector3::Zero;
};

class ParticleParticleContactGenerator : public ParticleContactGenerator
{
public:
	int AddContact(ParticleContact* contact, const int& limit) const override;

private:
	static bool particlePairUsed(const std::vector<std::tuple<Particle*, Particle*>>& particlePairs, Particle* one, Particle* two);
	static void shouldBeDestroyed(Particle* lhs, Particle* rhs, bool& outDestroyLhs, bool& outDestroyRhs);
};