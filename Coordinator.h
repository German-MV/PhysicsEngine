#pragma once

#include "Model.h"
#include "PhysicsObject.h"

class Coordinator
{
public:
	//Coordinator() = default;

	virtual void coordinate(Model* model, PhysicsObject* object) const { }
};

