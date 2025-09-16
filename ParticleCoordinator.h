#pragma once
#include "Coordinator.h"
#include "Particle.h"

class ParticleCoordinator : public Coordinator
{
public:
	void coordinate(Model* model, PhysicsObject* object) const override
	{
		Particle* particle = ToParticle(object);
		if (!particle) return;

		model->position = particle->GetPosition();
	}
};

