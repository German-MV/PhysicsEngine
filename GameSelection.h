#pragma once
#include "GameObject.h"

class GameSelection
{
public:
	GameObject* object;
	glm::vec3 point;

	GameSelection() = default;
	GameSelection(GameObject* object, const glm::vec3& point)
		: object(object), point(point) {}

	int GetType() const
	{
		if (object == nullptr)
			return -1;

		if (object->GetPhysicsObject() == nullptr)
			return NO_PHYSICS;

		return object->GetType();
	}
};

const GameSelection NullSelection = GameSelection(nullptr, glm::vec3(0.0f));