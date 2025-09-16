#include "RigidBodySamples.h"

//#include "GeometrySamples.h"
RigidBody CreateRigidCube(
	float mass,
	float size
)
{
	return CreateRigidBox(mass, glm::vec3(size));
}

RigidBody CreateRigidBox(
	float mass,
	glm::vec3 size
)
{
	float w = size.x;
	float h = size.y;
	float d = size.z;

	float I1 = (1.0f / 12.0f) * mass * (h * h + d * d);
	float I2 = (1.0f / 12.0f) * mass * (w * w + d * d);
	float I3 = (1.0f / 12.0f) * mass * (w * w + h * h);

	RigidBody box(mass, I1, I2, I3);

	return box;
}

RigidBody CreateRigidSphere(
	float mass,
	float radius
)
{
	float I = (2.0f / 5.0f) * mass * radius * radius;

	RigidBody sphere(mass, I, I, I);

	return sphere;
}

RigidBody CreateVoidRigidSphere(
	float mass,
	float radius
)
{
	float I = (2.0f / 3.0f) * mass * radius * radius;

	RigidBody sphere(mass, I, I, I);

	return sphere;
}

RigidBody CreateRigidEllipsoid(
	float mass,
	glm::vec3 size
)
{
	float a = size.x;
	float b = size.y;
	float c = size.z;

	float I1 = (1.f / 5.f) * mass * (b * b + c * c);
	float I2 = (1.f / 5.f) * mass * (a * a + c * c);
	float I3 = (1.f / 5.f) * mass * (a * a + b * b);

	RigidBody ellipsoid(mass, I1, I2, I3);

	return ellipsoid;
}

RigidBody CreateRigidCylinder(
	float mass,
	float radius,
	float height
)
{
	float I1 = (1.f / 12.f) * mass * (3.f * radius * radius + height * height);
	float I2 = I1;
	float I3 = (1.f / 2.f) * mass * radius * radius;

	RigidBody cylinder(mass, I1, I2, I3);

	return cylinder;
}

RigidBody CreateRigidVoidCylinder(
	float mass,
	float innerRadius,
	float outerRadius,
	float height
)
{
	// float effRadius = sqrt(innerRadius*innerRadius + outerRadius*outerRadius);
	// return CreateRigidCylinder(mass, effRadius, height);

	float effRadiusSq = outerRadius * outerRadius + innerRadius * innerRadius;

	float I1 = (1.f / 12.f) * mass * (3.f * effRadiusSq + height * height);
	float I2 = I1;
	float I3 = (1.f / 2.f) * mass * effRadiusSq;

	RigidBody cylinder(mass, I1, I2, I3);

	return cylinder;
}

RigidBody CreateRigidCone(
	float mass,
	float radius,
	float height
)
{
	float I1 = (3.f / 80.f) * mass * radius * radius + (3.f / 20.f) * mass * height * height; // PENDIENTE MIRAR SI ES CORRECTO
	float I2 = I1;
	float I3 = (3.f / 10.f) * mass * radius * radius;

	RigidBody cone(mass, I1, I2, I3);

	return cone;
}

RigidBody CreateTHandle(
	float mass,
	float cylinderLength1,
	float cylinderLength2,
	float radius1,
	float radius2
)
{
	float density = mass / (cylinderLength1 + cylinderLength2);
	float mass1 = density * cylinderLength1;
	float mass2 = density * cylinderLength2;

	RigidBody cylinder1 = CreateRigidCylinder(mass1, radius1, cylinderLength1);
	RigidBody cylinder2 = CreateRigidCylinder(mass2, radius2, cylinderLength2);

	cylinder1.SetPosition(glm::vec3(0.f, 0.f, 0.f));
	cylinder2.SetPosition(cylinder1.GetPosition() + glm::vec3(0.f, 0.f, 0.5f * cylinderLength2));

	cylinder1.SetOrientation(glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f)));
	cylinder2.SetOrientation(glm::angleAxis(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)));

	RigidBody tHandle = MergeRigidBodies(cylinder1, cylinder2);
	tHandle.SetPosition(glm::vec3(0.f, 0.f, 0.f));

	return tHandle;
}

RigidBody CreateTHandle(
	float m,
	float L,
	float R
)
{
	// https://github.com/CragAddict/Intermediate/blob/master/TbarHandlefinished.py

	float crossMass = 2.f * m;
	float crossLength = 2.f * L;
	float perpMass = m / 2.f;
	float perpLength = L;
	float perpRadius = 3*R/4.f;
	float crossRadius = R;

	float q = -(perpLength / 6.f + crossRadius / 3.f);

	float I1 = (1 / 2.f) * (crossMass) * (crossRadius * crossRadius) + (crossMass)*q * q + (perpMass / 4.f) * (perpRadius * perpRadius) + (perpMass / 12.f) * (perpLength * perpLength) + perpMass * (perpLength / 2.f + crossRadius - q);
	float I2 = (1 / 4.f) * (crossMass) * (crossRadius * crossRadius) + (1 / 12.f) * (crossMass) * (crossLength * crossLength) + crossMass * (q * q) + (1 / 4.f) * perpMass * (perpRadius * perpRadius) + (1 / 12.f) * (perpMass) * (perpLength * perpLength) + perpMass * (perpLength / 2.f + crossRadius - q);
	float I3 = (1 / 2.f) * (perpMass) * (perpRadius * perpRadius) + (1 / 4.f) * (crossMass) * (crossRadius * crossRadius) + (1 / 12.f) * (crossMass) * (crossLength * crossLength);

	// https://www.youtube.com/watch?v=PY7fRozbrtk

	//float q = - (L / 5.f + 2 * R / 5.f);

	/*float I1 =
		m * R * R + 2 * m * q * q +
		(1 / 8.f) * m * (3 * R / 4.f) * (3 * R / 4.f) +
		(1 / 12.f) * (m / 2.f) * L * L +
		(m / 2.f) * (L / 2.f + R + q) * (L / 2.f + R + q);

	float I2 =
		(m / 2.f) * R * R + 2 * m * q * q +
		(1 / 12.f) * (2 * m) * (2 * L) * (2 * L) +
		(1 / 8.f) * m * (3 * R / 4.f) * (3 * R / 4.f) +
		(1 / 12.f) * (m / 2.f) * L * L + 
		(m / 2.f) * (L / 2.f + R - q) * (L / 2.f + R - q);

	float I3 =
		(m / 4.f) * (3 * R / 4.f) * (3 * R / 4.f) +
		(1 / 2.f) * m * R * R +
		(1 / 6.f) * m * (2 * L) * (2 * L);*/

	RigidBody tHandle(2 * m + m / 2.f, I1, I2, I3);

	tHandle.SetPosition(glm::vec3(0.f, 0.f, 0.f));

	return tHandle;
}