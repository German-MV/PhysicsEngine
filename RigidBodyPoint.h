#pragma once
#include "ApplicationPoint.h"
#include "RigidBody.h"

class RigidBodyPoint : public ApplicationPoint
{
protected:
	glm::vec3 point;
	RigidBody* rigidBody;

public:
	RigidBodyPoint(RigidBody& rigidBody, glm::vec3 point);

	void AddForce(const glm::vec3& newForce) override;

	glm::vec3 GetPosition() const override;

	glm::vec3 GetVelocity() const override;

	RigidBody* GetRigidBody() const { return rigidBody; }
};
