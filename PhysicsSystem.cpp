#include "PhysicsSystem.h"

#include <algorithm>
#include <iostream>

//#define ASYNC

#ifdef ASYNC
#include <future>

void PhysicsSystem::Update(float deltaTime)
{
	// Interactions
	/*std::vector<std::future<void>> springFutures;
	for (Spring* spring : springs)
	{
		springFutures.push_back(std::async(std::launch::async, [spring]() {
			spring->applyForce();
		}));
	}

	for (std::future<void>& f : springFutures)
		f.get();*/
	for (Spring* spring : springs)
		spring->applyForce();

	// Updates
	std::vector<std::future<void>> rigidBodyFutures;
	for (RigidBody* body : rigidBodies)
	{
		rigidBodyFutures.push_back(std::async(std::launch::async, [body, deltaTime]() {
			body->Update(deltaTime);
		}));
	}
	for (std::future<void>& f : rigidBodyFutures)
		f.get();
	std::vector<std::future<void>> particleFutures;
	for (Particle* particle : particles)
	{
		particleFutures.push_back(std::async(std::launch::async, [particle, deltaTime]() {
			particle->Update(deltaTime);
		}));
	}
	for (std::future<void>& f : particleFutures)
		f.get();
	std::vector<std::future<void>> clothFutures;
	for (Cloth* cloth : cloths)
	{
		clothFutures.push_back(std::async(std::launch::async, [cloth, deltaTime]() {
			cloth->Update(deltaTime);
		}));
	}
	for (std::future<void>& f : clothFutures)
		f.get();
}

#else // ASYNC
void PhysicsSystem::Update(float deltaTime)
{
	// Interactions
	for (Spring* spring : springs)
		spring->applyForce();
	
	// Updates
	for (RigidBody* body : rigidBodies)
		body->Update(deltaTime);

	for (Particle* particle : particles)
		particle->Update(deltaTime);

	for (Cloth* cloth : cloths)
		cloth->Update(deltaTime);
}
#endif // ASYNC

/*void PhysicsSystem::Render(Shader& shader, const char* uniformName)
{
	for (RigidBody*& body : rigidBodies)
		body->Render(shader, uniformName);
}*/

bool PhysicsSystem::AddObject(PhysicsObject* object)
{
	switch (object->GetType())
	{
	case RIGID_BODY:
		return AddObject(*ToRigidBody(object));

	case PARTICLE:
		return AddObject(*ToParticle(object));

	case CLOTH:
		return AddObject(*ToCloth(object));

	case SPRING:
		return AddObject(*ToSpring(object));

	case APPLICATION_POINT: case RIGID_BODY_POINT:
		return true;
	}

	std::cout << "The object is not a valid physics object." << std::endl;
	return false;
}

bool PhysicsSystem::AddObject(RigidBody& body)
{
	if (std::find(rigidBodies.begin(), rigidBodies.end(), &body) != rigidBodies.end())
	{
		std::cout << "The rigid body number " << rigidBodies.size() << " already exists." << std::endl;
		return false;
	}

	rigidBodies.push_back(&body);
	return true;
}

bool PhysicsSystem::AddObject(Particle& particle)
{
	if (HasParticle(&particle))
	{
		std::cout << "The particle number " << particles.size() << " already exists." << std::endl;
		return false;
	}

	particles.push_back(&particle);
	return true;
}

bool PhysicsSystem::AddObject(Spring& spring)
{
	if (!HasParticle(spring.p1) || !HasParticle(spring.p2))
	{
		std::cout << "The particles does not exist in current physic system." << std::endl;
		return false;
	}

	//std::cout << "Added spring: " << &spring << std::endl;

	springs.push_back(&spring);
	return true;
}

bool PhysicsSystem::AddObject(Cloth& cloth)
{
	if (std::find(cloths.begin(), cloths.end(), &cloth) != cloths.end())
	{
		std::cout << "The cloth number " << cloths.size() << " already exists." << std::endl;
		return false;
	}

	for (Particle& particle : cloth.particles)
	{
		if (HasParticle(&particle))
		{
			std::cout << "A particle in the cloth already exists in the physics system." << std::endl;
			return false;
		}
	}

	cloths.push_back(&cloth);
	return true;
}

void PhysicsSystem::RemoveObject(PhysicsObject* object)
{
	RigidBody* rigidBody = dynamic_cast<RigidBody*>(object);
	if (rigidBody != nullptr)
	{
		RemoveObject(*rigidBody);
		return;
	}

	Particle* particle = dynamic_cast<Particle*>(object);
	if (particle != nullptr)
	{
		RemoveObject(*particle);
		return;
	}

	Spring* spring = dynamic_cast<Spring*>(object);
	if (spring != nullptr)
	{
		RemoveObject(*spring);
		return;
	}

	std::cout << "The object is not a valid physics object." << std::endl;
}

void PhysicsSystem::RemoveObject(const RigidBody& body)
{
	auto ref = std::find(rigidBodies.begin(), rigidBodies.end(), &body);
	if (ref == rigidBodies.end()) return;
	rigidBodies.erase(ref);
}

void PhysicsSystem::RemoveObject(const Particle& particle)
{
	auto ref = std::find(particles.begin(), particles.end(), &particle);
	if (ref == particles.end()) return;
	particles.erase(ref);


	for (auto it = springs.begin(); it < springs.end(); it++)
	{
		if (
			dynamic_cast<Particle*>((*it)->p1) == &particle || 
			dynamic_cast<Particle*>((*it)->p2) == &particle
		)
		{
			//springs.erase(it);
			//it--;
			RemoveObject(**it);
		}
	}
}

void PhysicsSystem::RemoveObject(const Spring& spring)
{
	auto ref = std::find(springs.begin(), springs.end(), &spring);
	if (ref == springs.end()) return;
	springs.erase(ref);
}

void PhysicsSystem::RemoveObject(const Cloth& cloth)
{
	auto ref = std::find(cloths.begin(), cloths.end(), &cloth);
	if (ref == cloths.end()) return;
	cloths.erase(ref);

	for (auto it = springs.begin(); it < springs.end(); it ++)
	{
		if (cloth.HasParticle(dynamic_cast<Particle*>((*it)->p1)) || cloth.HasParticle(dynamic_cast<Particle*>((*it)->p2)))
		{
			springs.erase(it);
			it--;
		}
	}
}

bool PhysicsSystem::HasParticle(const ApplicationPoint* point) const
{	
	switch (point->GetType())
	{
	case RIGID_BODY_POINT:
	{
		const RigidBodyPoint* rbp = dynamic_cast<const RigidBodyPoint*>(point);
		if (rbp == nullptr) return false;

		for (const RigidBody* body : rigidBodies)
		{
			if (rbp->GetRigidBody() == body)
				return true;
		}

		break;
	}

	case PARTICLE:
	{

		const Particle* particle = dynamic_cast<const Particle*>(point);

		if (std::find(particles.begin(), particles.end(), particle) != particles.end()) return true;
		
		for (Cloth* cloth : cloths)
		{
			if (cloth->HasParticle(particle))
				return true;
		}
		
		break;
	}

	case APPLICATION_POINT:
	{
		return true;
	}
	}

	return false;
}


//void PhysicsSystem::AddConstraint(const OBB& constraint)
//{
//	constraints.push_back(constraint);
//}

void PhysicsSystem::ClearObjects()
{
	rigidBodies.clear();
}

//void PhysicsSystem::ClearConstraints()
//{
//	constraints.clear();
//}