#include "RigidBody.h"

#ifndef ROTATIONAL_EULER

RigidBody::RotationalState RigidBody::RotationDerivative(const RigidBody::RotationalState& state)
{
	// Compute quaternion derivative
	glm::quat omegaQuat = glm::quat(0.0f, state.w);
	glm::quat dq = 0.5f * state.q * omegaQuat;

	// Compute angular acceleration
	glm::vec3 alpha;
	alpha.x = (torque.x + (I2 - I3) * state.w.y * state.w.z) / I1;
	alpha.y = (torque.y + (I3 - I1) * state.w.x * state.w.z) / I2;
	alpha.z = (torque.z + (I1 - I2) * state.w.x * state.w.y) / I3;

	return { dq, alpha };
}

#endif // ROTATIONAL_EULER

#ifndef TRANSLATIONAL_EULER
RigidBody::TranslationalState RigidBody::TranslationDerivative(const RigidBody::TranslationalState& state)
{
	glm::vec3 acceleration = force / mass;
	return { state.velocity, acceleration };
}
#endif // TRANSLATIONAL_EULER

RigidBody::RigidBody() : PhysicsObject(RIGID_BODY) {};

RigidBody::RigidBody(
	float mass,
	float I1, float I2, float I3
) :
	mass(mass),
	I1(I1), I2(I2), I3(I3),

	position(glm::vec3(0.f)),
	orientation(glm::quat(1.f, 0.f, 0.f, 0.f)),

	velocity(glm::vec3(0.f)),
	angularVelocity(glm::vec3(0.f)),

	force(glm::vec3(0.f)),
	torque(glm::vec3(0.f)),
	PhysicsObject(RIGID_BODY)
{}

#ifdef TRANSLATIONAL_EULER

void RigidBody::UpdateTranslation(float deltaTime)
{
	glm::vec3 acceleration = force / mass;
	velocity += acceleration * deltaTime;
	position += velocity * deltaTime;

	force = glm::vec3(0.f);
}

#endif // TRANSLATIONAL_EULER

#ifdef TRANSLATIONAL_RK4

void RigidBody::UpdateTranslation(float deltaTime)
{
	TranslationalState y0 = { position, velocity };

	TranslationalState k1 = TranslationDerivative(y0);

	TranslationalState y1 = { position + k1.position * (deltaTime * 0.5f), velocity + k1.velocity * (deltaTime * 0.5f) };
	TranslationalState k2 = TranslationDerivative(y1);

	TranslationalState y2 = { position + k2.position * (deltaTime * 0.5f), velocity + k2.velocity * (deltaTime * 0.5f) };
	TranslationalState k3 = TranslationDerivative(y2);

	TranslationalState y3 = { position + k3.position * deltaTime, velocity + k3.velocity * deltaTime };
	TranslationalState k4 = TranslationDerivative(y3);

	position += (1.0f / 6.0f) * (k1.position + 2.0f * k2.position + 2.0f * k3.position + k4.position) * deltaTime;
	velocity += (1.0f / 6.0f) * (k1.velocity + 2.0f * k2.velocity + 2.0f * k3.velocity + k4.velocity) * deltaTime;

	force = glm::vec3(0.f);
}

#endif // TRANSLATIONAL_RK4

#ifdef ROTATIONAL_EULER
void RigidBody::UpdateRotation(float deltaTime)
{
	torque = glm::inverse(orientation) * torque;

	glm::vec3 angularAcceleration;
	angularAcceleration.x = (torque.x + (I2 - I3) * angularVelocity.y * angularVelocity.z) / I1;
	angularAcceleration.y = (torque.y + (I3 - I1) * angularVelocity.x * angularVelocity.z) / I2;
	angularAcceleration.z = (torque.z + (I1 - I2) * angularVelocity.x * angularVelocity.y) / I3;

	// Update orientation and angular velocity
	angularVelocity += angularAcceleration * deltaTime;
	//orientation += 0.5f * glm::quat(0.0f, orientation * angularVelocity) * orientation * deltaTime;
	orientation += 0.5f * orientation * glm::quat(0.0f, angularVelocity) * deltaTime;
	//orientation = glm::angleAxis(glm::length(angularVelocity) * deltaTime, glm::normalize(orientation * angularVelocity)) * orientation;
	//orientation = orientation * glm::angleAxis(glm::length(angularVelocity) * deltaTime, glm::normalize(angularVelocity));

	orientation = glm::normalize(orientation);

	torque = glm::vec3(0.f);
}

#endif // ROTATIONAL_EULER

#ifdef ROTATIONAL_RK4

void RigidBody::UpdateRotation(float deltaTime)
{
	// Transform torque to principal inertia axises coordinates
	torque = glm::inverse(orientation) * torque;

	// RK4 integration
	RotationalState y0 = { orientation, angularVelocity };

	RotationalState k1 = RotationDerivative(y0);

	RotationalState y1 = { orientation + k1.q * (deltaTime * 0.5f), angularVelocity + k1.w * (deltaTime * 0.5f) };
	RotationalState k2 = RotationDerivative(y1);

	RotationalState y2 = { orientation + k2.q * (deltaTime * 0.5f), angularVelocity + k2.w * (deltaTime * 0.5f) };
	RotationalState k3 = RotationDerivative(y2);

	RotationalState y3 = { orientation + k3.q * deltaTime, angularVelocity + k3.w * deltaTime };
	RotationalState k4 = RotationDerivative(y3);

	orientation += (1.0f / 6.0f) * (k1.q + 2.0f * k2.q + 2.0f * k3.q + k4.q) * deltaTime;
	angularVelocity += (1.0f / 6.0f) * (k1.w + 2.0f * k2.w + 2.0f * k3.w + k4.w) * deltaTime;

	orientation = glm::normalize(orientation);

	torque = glm::vec3(0.f);
}

#endif // ROTATIONAL_RK4

void RigidBody::Update(float deltaTime)
{
	// TRASLATION:
	force -= velocity * damping;
	UpdateTranslation(deltaTime);

	// ROTATION:
	torque -= GetAngularVelocity() * angularDamping;
	UpdateRotation(deltaTime);

}

void RigidBody::ApplyAcceleration(glm::vec3 acceleration)
{
	force += acceleration * mass;
}

void RigidBody::ApplyForce(glm::vec3 newForce, glm::vec3 point)
{
	force += newForce;
	ApplyTorque(glm::cross(point, newForce));
}

void RigidBody::ApplyTorque(glm::vec3 newTorque)
{
	torque += newTorque;
}

void RigidBody::ApplyImpulse(glm::vec3 impulse, glm::vec3 point)
{
	velocity += impulse;
	angularVelocity += glm::cross(point/* - position*/, impulse);
}

void RigidBody::ResetForces()
{
	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);
}

//void RigidBody::SolveConstraints(const std::vector<OBB>& constraints)
//{
//	
//}

//void RigidBody::SetInertiaTensor(const glm::mat3& inertiaTensor)
//{
//	this->inertiaTensor = inertiaTensor;
//	inverseInertiaTensor = glm::inverse(inertiaTensor);
//}

void RigidBody::SetMass(float mass)
{
	this->mass = mass;
}

void RigidBody::SetInertiaTensor(float I1, float I2, float I3)
{
	this->I1 = I1;
	this->I2 = I2;
	this->I3 = I3;
}

void RigidBody::SetDamping(float damping)
{
	this->damping = damping;
}

void RigidBody::SetAngularDamping(float angularDamping)
{
	this->angularDamping = angularDamping;
}

void RigidBody::SetPosition(const glm::vec3& position)
{
	this->position = position;
}

void RigidBody::SetOrientation(const glm::quat& orientation)
{
	this->orientation = orientation;
}

void RigidBody::SetVelocity(const glm::vec3& velocity)
{
	this->velocity = velocity;
}

void RigidBody::SetLocalAngularVelocity(const glm::vec3& localAngularVelocity)
{
	this->angularVelocity = localAngularVelocity;
}

void RigidBody::SetAngularVelocity(const glm::vec3& angularVelocity)
{
	this->angularVelocity = glm::inverse(orientation) * angularVelocity;
}

float RigidBody::GetMass() const
{
	return mass;
}

float RigidBody::GetI1() const
{
	return I1;
}

float RigidBody::GetI2() const
{
	return I2;
}

float RigidBody::GetI3() const
{
	return I3;
}

glm::vec3 RigidBody::GetInertiaDiag() const
{
	return glm::vec3(I1, I2, I3);
}

float RigidBody::GetDamping() const
{
	return damping;
}

float RigidBody::GetAngularDamping() const
{
	return angularDamping;
}

glm::vec3 RigidBody::GetPosition() const
{
	return position;
}

glm::quat RigidBody::GetOrientation() const
{
	return orientation;
}

glm::vec3 RigidBody::GetVelocity() const
{
	return velocity;
}

glm::vec3 RigidBody::GetLocalAngularVelocity() const
{
	return angularVelocity;
}

glm::vec3 RigidBody::GetAngularVelocity() const
{
	return orientation * angularVelocity;
}

glm::vec3 RigidBody::GetForce() const
{
	return force;
}

glm::vec3 RigidBody::GetTorque() const
{
	return torque;
}


glm::vec3 RigidBody::GetWorldAngularMomentum() const
{
	return orientation * (GetDiagInertiaTensor() * angularVelocity);
}

glm::mat3 RigidBody::GetDiagInertiaTensor() const
{
	return glm::mat3(
		I1, 0.f, 0.f,
		0.f, I2, 0.f,
		0.f, 0.f, I3
	);
}

glm::mat3 RigidBody::GetWorldInertiaTensor() const
{
	glm::mat3 rotation = glm::mat3_cast(orientation);

	return rotation * GetDiagInertiaTensor() * glm::transpose(rotation);
}

glm::vec3 RigidBody::TransformPointToWorld(glm::vec3 rigidBodyPoint) const
{
	return position + orientation * rigidBodyPoint;
}


#include <Eigen/Eigenvalues>

void diagonalizeInertiaTensor(const glm::mat3& matrix, glm::vec3& eigenvalues, glm::mat3& eigenvectors)
{
	Eigen::Matrix3d mat;
	mat <<	matrix[0][0], matrix[0][1], matrix[0][2],
			matrix[1][0], matrix[1][1], matrix[1][2],
			matrix[2][0], matrix[2][1], matrix[2][2];

	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(mat);
	
	eigenvalues = glm::vec3(solver.eigenvalues()[0], solver.eigenvalues()[1], solver.eigenvalues()[2]);
	eigenvectors = glm::mat3(
		glm::normalize(glm::vec3(solver.eigenvectors()(0, 0), solver.eigenvectors()(1, 0), solver.eigenvectors()(2, 0))),
		glm::normalize(glm::vec3(solver.eigenvectors()(0, 1), solver.eigenvectors()(1, 1), solver.eigenvectors()(2, 1))),
		glm::normalize(glm::vec3(solver.eigenvectors()(0, 2), solver.eigenvectors()(1, 2), solver.eigenvectors()(2, 2)))
	);
}

RigidBody MergeRigidBodies(
	const RigidBody& bodyA,
	const RigidBody& bodyB
) {
	RigidBody mergedBody;

	// Translation parameters
	mergedBody.mass = bodyA.mass + bodyB.mass;
	mergedBody.position = (bodyA.position * bodyA.mass + bodyB.position * bodyB.mass) / mergedBody.mass;
	mergedBody.velocity = (bodyA.velocity * bodyA.mass + bodyB.velocity * bodyB.mass) / mergedBody.mass;

	// Rotation parameters
	glm::mat3 diagInertiaTensorA = glm::mat3(
		bodyA.I1, 0.0f, 0.0f,
		0.0f, bodyA.I2, 0.0f,
		0.0f, 0.0f, bodyA.I3
	);

	glm::mat3 diagInertiaTensorB = glm::mat3(
		bodyB.I1, 0.0f, 0.0f,
		0.0f, bodyB.I2, 0.0f,
		0.0f, 0.0f, bodyB.I3
	);

	// Calculate world inertia tensors
	glm::mat3 rotationA = glm::mat3_cast(bodyA.orientation);
	glm::mat3 rotationB = glm::mat3_cast(bodyB.orientation);

	glm::mat3 inertiaTensorA = rotationA * diagInertiaTensorA * glm::transpose(rotationA);
	glm::mat3 inertiaTensorB = rotationB * diagInertiaTensorB * glm::transpose(rotationB);
	
	// Apply Steiner's theorem
	glm::vec3 relPosA = bodyA.position - mergedBody.position;
	glm::vec3 relPosB = bodyB.position - mergedBody.position;

	glm::mat3 shiftedInertiaTensorA = inertiaTensorA + bodyA.mass * (glm::mat3(glm::dot(relPosA, relPosA)) - glm::outerProduct(relPosA, relPosA));
	glm::mat3 shiftedInertiaTensorB = inertiaTensorB + bodyB.mass * (glm::mat3(glm::dot(relPosB, relPosB)) - glm::outerProduct(relPosB, relPosB));

	glm::mat3 mergedInertiaTensor = shiftedInertiaTensorA + shiftedInertiaTensorB;
	
	// Diagonalize the inertia tensor
	glm::vec3 inertiaValues;
	glm::mat3 principalInertiaAxises;
	diagonalizeInertiaTensor(mergedInertiaTensor, inertiaValues, principalInertiaAxises);
	
	// normalize the principal inertia axises
	/*principalInertiaAxises[0] = glm::normalize(principalInertiaAxises[0]);
	principalInertiaAxises[1] = glm::normalize(principalInertiaAxises[1]);
	principalInertiaAxises[2] = glm::normalize(principalInertiaAxises[2]);*/

	// Calculate the new inertia tensor
	mergedBody.I1 = inertiaValues.x;
	mergedBody.I2 = inertiaValues.y;
	mergedBody.I3 = inertiaValues.z;

	// Calculate the new orientation
	mergedBody.orientation = glm::normalize(glm::quat(principalInertiaAxises));

	// Calculate the new angular velocity
	glm::vec3 angularMomentumA = bodyA.GetWorldAngularMomentum();
	glm::vec3 angularMomentumB = bodyB.GetWorldAngularMomentum();

	glm::vec3 shiftedAngularMomentumA = angularMomentumA + glm::cross(relPosA, bodyA.mass * bodyA.velocity);
	glm::vec3 shiftedAngularMomentumB = angularMomentumB + glm::cross(relPosB, bodyB.mass * bodyB.velocity);

	glm::vec3 mergedAngularMomentum = shiftedAngularMomentumA + shiftedAngularMomentumB;

	mergedBody.angularVelocity = (1.f / inertiaValues) * (glm::inverse(mergedBody.orientation) * mergedAngularMomentum);

	return mergedBody;
}

glm::vec3 GetPointNextPosition(const RigidBody& rigidBody, const glm::vec3 point, float deltaTime)
{
	RigidBody rigid = rigidBody;

	glm::vec3 relativePosition = point - rigid.GetPosition();
	glm::vec3 rigidPoint = glm::inverse(rigid.GetOrientation()) * relativePosition;

	rigid.Update(deltaTime);

	glm::vec3 result = rigid.GetOrientation() * rigidPoint + rigid.GetPosition();

	return result;
}

//void RigidBodyPoint::AddForce(glm::vec3 force)
//{
//	rigidBody->ApplyForce(force, rigidBody->orientation * point);
//}
//
//glm::vec3 RigidBodyPoint::GetPosition()
//{
//	return rigidBody->position + rigidBody->orientation * point;
//}
//
//glm::vec3 RigidBodyPoint::GetVelocity()
//{
//	return rigidBody->velocity + glm::cross(rigidBody->angularVelocity, point);
//}

RigidBody* ToRigidBody(PhysicsObject* physicsObject)
{
	if (physicsObject->GetType() == RIGID_BODY)
		return static_cast<RigidBody*>(physicsObject);
	
	return nullptr;
}