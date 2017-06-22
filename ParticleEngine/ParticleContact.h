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