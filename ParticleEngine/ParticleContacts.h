#pragma once

class ParticleContact
{
	friend class ParticleContactResolver;
public:
	/**
	* Holds the particles that are involved in the contact. The
	* second of these can be NULL, for contacts with the scenery.
	*/
	Particle* ContactParticles[2];

	/**
	* Holds the normal restitution coefficient at the contact.
	*/
	float Restitution;

	/**
	* Holds the direction of the contact in world coordinates.
	*/
	DirectX::SimpleMath::Vector3 ContactNormal;

	/**
	* Holds the depth of penetration at the contact.
	*/
	float Penetration;

	/**
	* Holds the amount each particle is moved by during interpenetration
	* resolution.
	*/
	DirectX::SimpleMath::Vector3 ParticleMovement[2];

protected:
	/**
	* Resolves this contact, for both velocity and interpenetration.
	*/
	void resolve(const float& deltaTime);

	/**
	* Calculates the separating velocity at this contact.
	*/
	float calculateSeparatingVelocity() const;

private:
	/**
	* Handles the impulse calculations for this collision.
	*/
	void resolveVelocity(const float& deltaTime);

	/**
	* Handles the interpenetration resolution for this contact.
	*/
	void resolveInterpenetration(const float& deltaTime);
};

/**
* The contact resolution routine for particle contacts. One
* resolver instance can be shared for the whole simulation.
*/
class ParticleContactResolver
{
public:
	/**
	* Creates a new contact resolver.
	*/
	ParticleContactResolver(const unsigned& iterations);

	/**
	* Sets the number of iterations that can be used.
	*/
	void SetIterations(const unsigned& iterations);

	/**
	* Resolves a set of particle contacts for both penetration
	* and velocity.
	*
	* Contacts that cannot interact with each other should be
	* passed to separate calls to resolveContacts, as the
	* resolution algorithm takes much longer for lots of contacts
	* than it does for the same number of contacts in small sets.
	*
	* @param contactArray Pointer to an array of particle contact
	* objects.
	*
	* @param numContacts The number of contacts in the array to
	* resolve.
	*
	* @param numIterations The number of iterations through the
	* resolution algorithm. This should be at least the number of
	* contacts (otherwise some constraints will not be resolved -
	* although sometimes this is not noticable). If the
	* iterations are not needed they will not be used, so adding
	* more iterations may not make any difference. But in some
	* cases you would need millions of iterations. Think about
	* the number of iterations as a bound: if you specify a large
	* number, sometimes the algorithm WILL use it, and you may
	* drop frames.
	*
	* @param duration The duration of the previous integration step.
	* This is used to compensate for forces applied.
	*/
	void ResolveContacts(ParticleContact *contactArray,	const int& numContacts,	const float& duration);

protected:
	/**
	* Holds the number of iterations allowed.
	*/
	unsigned m_iterations = 0;

	/**
	* This is a performance tracking value - we keep a record
	* of the actual number of iterations used.
	*/
	unsigned m_iterationsUsed = 0;
};