#include "Spring.h"
#define ALMOST_ZERO 1E-6
//template<typename T1, typename T2>
//void Spring::computeForce(T1* p1, T2* p2)
//{
//	glm::vec3 particleToParticle = p2->GetPosition() - p1->GetPosition();
//	glm::vec3 relativeVelocity = p2->GetVelocity() - p1->GetVelocity();
//
//	glm::vec3 force;
//
//	float length = glm::length(particleToParticle);
//	force = (k * (length - restingLength)) * glm::normalize(particleToParticle) + damping * relativeVelocity;
//
//	p1->AddForce(force);
//	p2->AddForce(-force);
//}

//#include "RigidBodyPoint.h"
//#include "ClothPoint.h"
//#include "RopePoint.h"

void Spring::applyForce()
{
	glm::vec3 relativePosition = p2->GetPosition() - p1->GetPosition();
	glm::vec3 relativeVelocity = p2->GetVelocity() - p1->GetVelocity();

	glm::vec3 force;

	float length = glm::length(relativePosition);

	if (fabsf(length) < ALMOST_ZERO) {
		return;
	}

	force = (k * (length - restingLength)) * glm::normalize(relativePosition) + damping * relativeVelocity;

	p1->AddForce(force);
	p2->AddForce(-force);
}


glm::vec3 GetSpringRelativePositions(const Spring& spring)
{
	glm::vec3 p1 = spring.p1->GetPosition();
	glm::vec3 p2 = spring.p2->GetPosition();

	return p2 - p1;
}

glm::vec3 GetSpringCenter(const Spring& spring)
{
	glm::vec3 p1 = spring.p1->GetPosition();
	glm::vec3 p2 = spring.p2->GetPosition();
	return (p1 + p2) * 0.5f;
}

Spring* ToSpring(PhysicsObject* object)
{
	if (object->GetType() == SPRING)
		return static_cast<Spring*>(object);
	else
		return nullptr;
}