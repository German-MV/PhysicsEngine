#pragma once
#include "Coordinator.h"
#include "RigidBody.h"

class RigidBodyCoordinator : public Coordinator
{
public:
	glm::vec3 positionOffset;
	glm::quat orientationOffset;

	inline RigidBodyCoordinator(glm::vec3 positionOffset, glm::quat orientationOffset) : positionOffset(positionOffset), orientationOffset(orientationOffset) {}

	inline RigidBodyCoordinator(const Model& model, const RigidBody& rigidBody)
	{
		positionOffset = model.position - rigidBody.GetPosition();
		orientationOffset = glm::inverse(rigidBody.GetOrientation()) * model.orientation;
	}

	inline void coordinate(Model* model, PhysicsObject* object) const
	{
		RigidBody* rigidBody = dynamic_cast<RigidBody*>(object);
		if (!rigidBody) return;
		model->position = rigidBody->GetPosition() + rigidBody->GetOrientation() * positionOffset;
		model->orientation = rigidBody->GetOrientation() * orientationOffset;
	}
};

