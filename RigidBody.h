#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "PhysicsObject.h"

//#define ROTATIONAL_EULER
//#define TRANSLATIONAL_EULER

#ifndef ROTATIONAL_EULER
#define ROTATIONAL_RK4
#endif // ROTATIONAL_EULER

#ifndef TRANSLATIONAL_EULER
#define TRANSLATIONAL_RK4
#endif // TRANSLATIONAL_EULER

class RigidBody : public PhysicsObject
{
protected:
	glm::vec3 force = glm::vec3(0.f);
	glm::vec3 torque = glm::vec3(0.f);

	float mass;
	float I1, I2, I3;  // inertia tensor diagonal elements

	float damping = 0.f;
	float angularDamping = 0.f;

	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 velocity;
	glm::vec3 angularVelocity; // object space

	friend RigidBody MergeRigidBodies(
		const RigidBody& bodyA,
		const RigidBody& bodyB
	);

#ifndef ROTATIONAL_EULER

	struct RotationalState
	{
		glm::quat q;
		glm::vec3 w;
	};

	RotationalState RotationDerivative(const RotationalState& state);

#endif // ROTATIONAL_EULER

#ifndef TRANSLATIONAL_EULER

	struct TranslationalState
	{
		glm::vec3 position;
		glm::vec3 velocity;
	};

	TranslationalState TranslationDerivative(const TranslationalState& state);

#endif // TRANSLATIONAL_EULER

	void UpdateTranslation(float deltaTime);
	void UpdateRotation(float deltaTime);

public:
	RigidBody();

	RigidBody(
		float mass,
		float I1, float I2, float I3
	);

	void Update(float deltaTime);

	void ApplyAcceleration(glm::vec3 acceleration);

	void ApplyForce(glm::vec3 newForce, glm::vec3 point);

	void ApplyTorque(glm::vec3 newTorque);

	void ApplyImpulse(glm::vec3 impulse, glm::vec3 point);

	void ResetForces();

	//void SolveConstraints(const std::vector<OBB>& constraints);
	
	//void SetInertiaTensor(float I1, float I2, float I3);

	void SetMass(float mass);
	void SetInertiaTensor(float I1, float I2, float I3);
	
	void SetDamping(float damping);
	void SetAngularDamping(float angularDamping);

	void SetPosition(const glm::vec3& position);
	void SetOrientation(const glm::quat& orientation);
	void SetVelocity(const glm::vec3& velocity);
	void SetLocalAngularVelocity(const glm::vec3& localAngularVelocity);
	void SetAngularVelocity(const glm::vec3& angularVelocity);

	float GetMass() const;
	float GetI1() const;
	float GetI2() const;
	float GetI3() const;
	glm::vec3 GetInertiaDiag() const;

	float GetDamping() const;
	float GetAngularDamping() const;

	glm::vec3 GetPosition() const;
	glm::quat GetOrientation() const;
	glm::vec3 GetVelocity() const;
	glm::vec3 GetLocalAngularVelocity() const;
	glm::vec3 GetAngularVelocity() const;

	glm::vec3 GetForce() const;
	glm::vec3 GetTorque() const;

	glm::vec3 GetWorldAngularMomentum() const;

	glm::mat3 GetDiagInertiaTensor() const;
	glm::mat3 GetWorldInertiaTensor() const;


	glm::vec3 TransformPointToWorld(glm::vec3 rigidBodyPoint) const;
};

void diagonalizeInertiaTensor(const glm::mat3& matrix, glm::vec3& eigenvalues, glm::mat3& eigenvectors);

RigidBody MergeRigidBodies(
	const RigidBody& bodyA,
	const RigidBody& bodyB
);

glm::vec3 GetPointNextPosition(const RigidBody& rigidBody, const glm::vec3 point, float deltaTime);

//void MovePointTo(RigidBody& rigidBody, const glm::vec3 point, const glm::vec3 to)
//{
//	
//}

RigidBody* ToRigidBody(PhysicsObject* physicsObject);