#pragma once
#include "GameObject.h"
#include "Cloth.h"
#include "ClothCoordinator.h"
#include "GeometrySamples.h"

class GameCloth : public GameObject
{
protected:
	void AddModel(Model* model)
	{
		models.push_back(model);
		coordinators[model] = new ClothCoordinator();
	}

public:
	GameCloth(int width, int height, float spacing, float mass = 1.f)
	{
		physics = new Cloth(width, height, spacing, mass);
		Mesh* mesh = CreateClothMesh(width, height, WHITE);
		Model* model = new Model(mesh);

		AddModel(model);
	}

	inline Cloth* GetPhysics()
	{
		return ToCloth(physics);
	}

	inline const Cloth* GetPhysics() const
	{
		return ToCloth(physics);
	}

	inline Cloth* operator->()
	{
		return GetPhysics();
	}

	inline const Cloth* operator->() const
	{
		return ToCloth(physics);
	}

	inline Model* GetModel() const
	{
		return models[0];
	}

	inline Mesh* GetMesh() const
	{
		return models[0]->GetMesh();
	}
};