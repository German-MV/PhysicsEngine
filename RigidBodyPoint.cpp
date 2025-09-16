#include "RigidBodyPoint.h"

RigidBodyPoint::RigidBodyPoint(RigidBody& rigidBody, glm::vec3 point) :
	rigidBody(&rigidBody),
	point(glm::inverse(rigidBody.GetOrientation())* (point - rigidBody.GetPosition())),
	ApplicationPoint(RIGID_BODY_POINT)
{}

void RigidBodyPoint::AddForce(const glm::vec3& newForce) 
{
	rigidBody->ApplyForce(newForce, rigidBody->GetOrientation() * point);
}

glm::vec3 RigidBodyPoint::GetPosition() const
{
	return rigidBody->GetPosition() + rigidBody->GetOrientation() * point;
}

glm::vec3 RigidBodyPoint::GetVelocity() const
{
	return rigidBody->GetVelocity() + rigidBody->GetOrientation() * glm::cross(rigidBody->GetLocalAngularVelocity(), point);
}