#pragma once

enum
{
	PHYSICS_OBJECT,
	APPLICATION_POINT,
	PARTICLE,
	SPRING,
	CLOTH,
	RIGID_BODY,
	RIGID_BODY_POINT,
	NO_PHYSICS
};

class PhysicsObject
{
protected:
	const int type;

	PhysicsObject(int type = PHYSICS_OBJECT) : type(type) {}

public:
	virtual void Update(float deltaTime) {}

	inline int GetType() const { return type; }
};
