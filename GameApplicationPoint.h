#pragma once

#include "GameObject.h"
#include "GameParticle.h"
#include "GameRigidBody.h"
#include "GameCloth.h"
#include "RigidBodyPoint.h"
#include "ApplicationPointCoordinator.h"

#include "DebugTools.h"

#define ALMOST_ZERO 1E-6

class GameApplicationPoint : public GameObject
{
protected:
	void SetModel(float radius)
	{
		//if (fabsf(radius) < ALMOST_ZERO) return;
		Mesh* mesh = CreateSphereMesh(5, 5, WHITE);
		Model* model = new Model(mesh);
		model->scale = glm::vec3(radius);
		AddModel(model);
	}

public:
	GameApplicationPoint()
	{
		physics = new ApplicationPoint();
	}

	GameApplicationPoint(GameRigidBody rigidBody, glm::vec3 point, float radius = 0.f)
	{
		physics = new RigidBodyPoint(*rigidBody.GetPhysics(), point);
		//std::cout << RigidBodyPoint(*rigidBody.GetPhysics(), point).GetPosition();
		//ToApplicationPoint(physics)->GetPosition();
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}

	GameApplicationPoint(GameCloth cloth, glm::vec3 point, float radius = 0.f)
	{
		physics = cloth->GetParticleAt(point);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}

	/*GameApplicationPoint(GameRope rope, glm::vec3 point, float radius = 0.f)
	{
		physics = rope->GetParticleAt(point);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}*/

	GameApplicationPoint(const glm::vec3& point, float radius = 0.f)
	{
		physics = new ApplicationPoint(point);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}


	void AddModel(Model* model)
	{
		models.push_back(model);
		coordinators[model] = new ApplicationPointCoordinator();
	}

	inline void MakeVisible(float radius)
	{
		SetModel(radius);
	}

	inline ApplicationPoint* GetPhysics()
	{
		return ToApplicationPoint(physics);
	}

	inline const ApplicationPoint* GetPhysics() const
	{
		return ToApplicationPoint(physics);
	}

	inline ApplicationPoint* operator->()
	{
		return GetPhysics();
	}

	inline const ApplicationPoint* operator->() const
	{
		return ToApplicationPoint(physics);
	}
};