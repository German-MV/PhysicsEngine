#include "Particle.h"

void Particle::Update(float deltaTime)
{
	if(!fixed)
	{
		force -= damping * velocity;
		glm::vec3 acceleration = force / mass;
		velocity += acceleration * deltaTime;
		position += velocity * deltaTime;
	}

	ResetForces();
}
void Particle::AddForce(const glm::vec3& newForce)
{
	force += newForce;
}

void Particle::ResetForces()
{
	force = glm::vec3(0.f);
}

Particle* ToParticle(PhysicsObject* obj)
{
	if (obj->GetType() == PARTICLE)
		return static_cast<Particle*>(obj);

	return nullptr;
}