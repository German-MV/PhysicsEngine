#pragma once

#include "RigidBody.h"

RigidBody CreateRigidCube(
	float mass,
	float size
);

RigidBody CreateRigidBox(
	float mass,
	glm::vec3 size
);

RigidBody CreateRigidSphere(
	float mass,
	float radius
);

RigidBody CreateVoidRigidSphere(
	float mass,
	float radius
);

RigidBody CreateRigidEllipsoid(
	float mass,
	glm::vec3 size
);

RigidBody CreateRigidCylinder(
	float mass,
	float radius,
	float height
);

RigidBody CreateRigidVoidCylinder(
	float mass,
	float innerRadius,
	float outerRadius,
	float height
);

RigidBody CreateRigidCone(
	float mass,
	float radius,
	float height
);

RigidBody CreateTHandle(
	float mass,
	float cylinderLength1,
	float cylinderLength2,
	float radius1,
	float radius2
);

RigidBody CreateTHandle(
	float m,
	float L,
	float R
);