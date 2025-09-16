#pragma once

#include "RigidBody.h"
#include "Particle.h"
#include "Spring.h"
#include "Cloth.h"
#include "RigidBodyPoint.h"
#include "Geometry3D.h"
#include <vector>

class PhysicsSystem
{
protected:
	//std::vector<ApplicationPoint*> applicationPoints;
	std::vector<RigidBody*> rigidBodies;
	std::vector<Particle*> particles;
	std::vector<Spring*> springs;
	std::vector<Cloth*> cloths;

	//std::vector<OBB> constraints;

public:
	void Update(float deltaTime);

	bool AddObject(PhysicsObject* object);
	bool AddObject(RigidBody& rigidBody);
	bool AddObject(Particle& particle);
	bool AddObject(Spring& spring);
	bool AddObject(Cloth& cloth);

	void RemoveObject(PhysicsObject* object);
	void RemoveObject(const RigidBody& rigidBody);
	void RemoveObject(const Particle& particle);
	void RemoveObject(const Spring& spring);
	void RemoveObject(const Cloth& cloth);

	bool HasParticle(const ApplicationPoint* particle) const;

	//void AddConstraint(const OBB& constraint);

	void ClearObjects();
	//void ClearConstraints();
};