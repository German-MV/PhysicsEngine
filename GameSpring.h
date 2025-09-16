#pragma once

#include "GameObject.h"
#include "Spring.h"
#include "SpringCoordinator.h"
#include "GameParticle.h"
#include "GameApplicationPoint.h"
#include "GeometrySamples.h"

class GameSpring : public GameObject
{
protected:
	void SetModel(float radius)
	{
		Mesh* mesh = CreateCylinderMesh(5, GREY);
		Model* model = new Model(mesh);
		model->scale = glm::vec3(radius, 0.f, radius);
		Coordinator* coordinator = new SpringCoordinator();
		AddModel(model, coordinator);
		Coordinate();
	}

	void SetPhysics(const Spring& spring)
	{
		physics = new Spring(spring);
	}

public:
	GameSpring()
	{
		Spring spring;
		SetPhysics(spring);
	}

	template <typename T1, typename T2>
	GameSpring(T1 point1, T2 point2, float k = 1.f, float restingLength = 0.f, float radius = 0.1f)
	{
		Spring spring(point1.GetPhysics(), point2.GetPhysics(), k, restingLength);

		SetPhysics(spring);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}

	/*template <typename T>
	GameSpring(T point1, glm::vec3 point2, float k = 1.f, float restingLength = 0.f, float radius = 0.f)
	{
		ApplicationPoint appPoint2(point2);
		Spring spring(point1.GetPhysics(), &appPoint2, k, restingLength);
		SetPhysics(spring);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}

	template <typename T>
	GameSpring(glm::vec3 point1, T point2, float k = 1.f, float restingLength = 0.f, float radius = 0.f)
	{
		ApplicationPoint appPoint1(point1);
		Spring spring(&appPoint1, point2.GetPhysics(), k, restingLength);
		SetPhysics(spring);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}*/

	GameSpring(const Spring& spring, float radius = 0.f)
	{
		SetPhysics(spring);
		if (fabsf(radius) > ALMOST_ZERO) SetModel(radius);
	}

	Spring* GetPhysics()
	{
		return ToSpring(physics);
	}

	const Spring* GetPhysics() const
	{
		return ToSpring(physics);
	}

	Spring* operator->()
	{
		return GetPhysics();
	}

	const Spring* operator->() const
	{
		return GetPhysics();
	}
};