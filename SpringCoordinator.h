#pragma once
#include "Coordinator.h"
#include "Spring.h"

class SpringCoordinator : public Coordinator
{
public:
	void coordinate(Model* model, PhysicsObject* physics) const override
	{
		Spring* spring = ToSpring(physics);
		if (!spring) return;

		glm::vec3 relativePos = GetSpringRelativePositions(*spring);
		model->position = GetSpringCenter(*spring);
		model->orientation = glm::quat(glm::vec3(0.f, 1.f, 0.f), relativePos);
		model->scale.y = glm::length(relativePos);
	}
};

