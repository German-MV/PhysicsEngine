#pragma once

#include <glm/glm.hpp>
#include "PhysicsObject.h"
#include "ApplicationPoint.h"

class Particle : public ApplicationPoint
{
protected:
	float mass;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;

	//glm::vec3 oldPosition;

	float damping = 1.f;

public:
	bool fixed = false;

	inline Particle() : mass(1.0f), position(0.0f), velocity(0.0f), force(0.0f), ApplicationPoint(PARTICLE) {}
	inline Particle(float mass) : mass(1.f), position(0.f), velocity(0.f), force(0.f), ApplicationPoint(PARTICLE) {}

	void Update(float deltaTime);

	void AddForce(const glm::vec3& newForce) override;
	void ResetForces();

	virtual inline glm::vec3 GetPosition() const { return position; }
	virtual inline glm::vec3 GetVelocity() const { return velocity; }
	virtual inline float GetMass() const { return mass; }

	virtual inline void SetPosition(glm::vec3 newPosition) { position = newPosition; }
	virtual inline void SetVelocity(glm::vec3 newVelocity) { velocity = newVelocity; }
	virtual inline void SetMass(float newMass) { mass = newMass; }
};

Particle* ToParticle(PhysicsObject* obj);