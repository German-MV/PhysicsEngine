#pragma once
#include "GameObject.h"
#include "RigidBody.h"
#include "RigidBodyCoordinator.h"

class GameRigidBody : public GameObject
{
public:
	GameRigidBody()
	{
		physics = new RigidBody(1.f, 1.f, 1.f, 1.f);
	}

	GameRigidBody(float mass, float I1, float I2, float I3)
	{
		physics = new RigidBody(mass, I1, I2, I3);
	}

	GameRigidBody(const RigidBody& rigidBody)
	{
		physics = new RigidBody(rigidBody);
	}

	void AddModel(Model* model)
	{
		models.push_back(model);
		coordinators[model] = new RigidBodyCoordinator(*model, *ToRigidBody(physics));
	}

	inline RigidBody* GetPhysics()
	{
		return ToRigidBody(physics);
	}

	inline const RigidBody* GetPhysics() const
	{
		return ToRigidBody(physics);
	}

	inline RigidBody* operator->()
	{
		return GetPhysics();
	}

	inline const RigidBody* operator->() const
	{
		return ToRigidBody(physics);
	}
};

GameRigidBody MergeRigidBodies(GameRigidBody& a, GameRigidBody& b);