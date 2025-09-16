#pragma once
#include "Coordinator.h"
#include "ApplicationPoint.h"

class ApplicationPointCoordinator : public Coordinator
{
public:
	void coordinate(Model* model, PhysicsObject* physics) const override
	{
		ApplicationPoint* appPoint = ToApplicationPoint(physics);
		if (!appPoint) return;

		model->position = appPoint->GetPosition();
	}
};

