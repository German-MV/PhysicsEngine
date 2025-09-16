#pragma once

#include <glm/glm.hpp>
#include "PhysicsObject.h"

class ApplicationPoint : public PhysicsObject
{
private:
	glm::vec3 position;

protected:
	inline ApplicationPoint(int type) : PhysicsObject(type) {}

public:
	ApplicationPoint() : PhysicsObject(APPLICATION_POINT) {}

	inline ApplicationPoint(glm::vec3 position) : PhysicsObject(APPLICATION_POINT), position(position) {}

	virtual inline glm::vec3 GetPosition() const { return glm::vec3(0.f); }

	virtual inline void AddForce(const glm::vec3& newForce) {}

	virtual inline glm::vec3 GetVelocity() const { return glm::vec3(0.f); }
};

inline ApplicationPoint* ToApplicationPoint(PhysicsObject* object)
{
	if (object->GetType() == APPLICATION_POINT || object->GetType() == RIGID_BODY_POINT || object->GetType() == PARTICLE)
		return static_cast<ApplicationPoint*>(object);
	else
		return nullptr;
}