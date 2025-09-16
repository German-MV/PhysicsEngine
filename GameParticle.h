#pragma once
#include "GameObject.h"
#include "Particle.h"
#include "ParticleCoordinator.h"
#include "GeometrySamples.h"

class GameParticle : public GameObject
{
public:
	GameParticle(float radius, float mass)
	{
		physics = new Particle(mass);

		Mesh* mesh = CreateSphereMesh(10, 10, WHITE);
		Model* model = new Model(mesh);
		model->scale = glm::vec3(radius);
		ParticleCoordinator* coordinator = new ParticleCoordinator();

		AddModel(model, coordinator);
	}

	/*~GameParticle()
	{
		delete physics;
		delete models[0]->GetMesh();
		delete models[0];
		delete coordinators[models[0]];
	}*/

	inline Particle* GetPhysics()
	{
		return ToParticle(physics);
	}

	inline const Particle* GetPhysics() const
	{
		return ToParticle(physics);
	}


	inline Particle* operator->()
	{
		return GetPhysics();
	}

	inline const Particle* operator->() const
	{
		return ToParticle(physics);
	}
};