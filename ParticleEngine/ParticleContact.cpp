#include "pch.h"
#include "ParticleContact.h"

using namespace DirectX::SimpleMath;

void ParticleContact::resolve(const float& deltaTime)
{
	resolveVelocity(deltaTime);
	resolveInterpenetration(deltaTime);
}

float ParticleContact::calculateSeparatingVelocity() const
{
	Vector3 relativeVelocity = ContactParticles[0]->GetVelocity();
	if (ContactParticles[1])
	{
		relativeVelocity -= ContactParticles[1]->GetVelocity();
	}
	return relativeVelocity.Dot(ContactNormal);
}

void ParticleContact::resolveVelocity(const float& deltaTime)
{
	// Find the velocity in the direction of the contact
	float separatingVelocity = calculateSeparatingVelocity();

	// Check if it needs to be resolved
	if (separatingVelocity > 0)
	{
		// The contact is either separating, or stationary - there's
		// no impulse required.
		return;
	}

	// Calculate the new separating velocity
	float newSepVelocity = -separatingVelocity * Restitution;

	// Check the velocity build-up due to acceleration only
	Vector3 accCausedVelocity = ContactParticles[0]->GetAcceleration();
	if (ContactParticles[1])
	{
		accCausedVelocity -= ContactParticles[1]->GetAcceleration();
	}
	float accCausedSepVelocity = accCausedVelocity.Dot(ContactNormal) * deltaTime;

	// If we've got a closing velocity due to acceleration build-up,
	// remove it from the new separating velocity
	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += Restitution * accCausedSepVelocity;

		// Make sure we haven't removed more than was
		// there to remove.
		if (newSepVelocity < 0)
		{
			newSepVelocity = 0;
		}
	}

	float deltaVelocity = newSepVelocity - separatingVelocity;

	// We apply the change in velocity to each object in proportion to
	// their inverse mass (i.e. those with lower inverse mass [higher
	// actual mass] get less change in velocity)..
	float totalInverseMass = ContactParticles[0]->GetInverseMass();
	if (ContactParticles[1]) totalInverseMass += ContactParticles[1]->GetInverseMass();

	// If all particles have infinite mass, then impulses have no effect
	if (totalInverseMass <= 0) return;

	// Calculate the impulse to apply
	float impulse = deltaVelocity / totalInverseMass;

	// Find the amount of impulse per unit of inverse mass
	Vector3 impulsePerIMass = ContactNormal * impulse;

	// Apply impulses: they are applied in the direction of the contact,
	// and are proportional to the inverse mass.
	ContactParticles[0]->SetVelocity(ContactParticles[0]->GetVelocity() +
		impulsePerIMass * ContactParticles[0]->GetInverseMass()
	);
	if (ContactParticles[1])
	{
		// Particle 1 goes in the opposite direction
		ContactParticles[1]->SetVelocity(ContactParticles[1]->GetVelocity() +
			impulsePerIMass * -ContactParticles[1]->GetInverseMass()
		);
	}
}

void ParticleContact::resolveInterpenetration(const float& deltaTime)
{
	// If we don't have any penetration, skip this step.
	if (Penetration <= 0) 
		return;

	// The movement of each object is based on their inverse mass, so
	// total that.
	float totalInverseMass = ContactParticles[0]->GetInverseMass();
	if (ContactParticles[1])
	{
		totalInverseMass += ContactParticles[1]->GetInverseMass();
	}

	// If all particles have infinite mass, then we do nothing
	if (totalInverseMass <= 0) 
		return;

	// Find the amount of penetration resolution per unit of inverse mass
	Vector3 movePerIMass = ContactNormal * (Penetration / totalInverseMass);

	// Calculate the the movement amounts
	ParticleMovement[0] = movePerIMass * ContactParticles[0]->GetInverseMass();
	if (ContactParticles[1]) 
	{
		ParticleMovement[1] = movePerIMass * -ContactParticles[1]->GetInverseMass();
	}
	else 
	{
		ParticleMovement[1] = Vector3::Zero;
	}

	// Apply the penetration resolution
	ContactParticles[0]->SetPosition(ContactParticles[0]->GetPosition() + ParticleMovement[0]);
	if (ContactParticles[1]) 
	{
		ContactParticles[1]->SetPosition(ContactParticles[1]->GetPosition() + ParticleMovement[1]);
	}
}