#pragma once

/**
* This is the basic polymorphic interface for contact generators
* applying to particles.
*/
class ParticleContactGenerator
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
	virtual int AddContact(ParticleContact *contact, const int& limit) const = 0;
};

/**
* A contact generator that takes an STL vector of particle pointers and
* collides them against the ground.
*/
class ParticleGroundContactsGenerator : public ParticleContactGenerator, public ParticleManagement
{
public:
	int AddContact(ParticleContact *contact, const int& limit) const override;
};