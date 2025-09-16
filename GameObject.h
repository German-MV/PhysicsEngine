#pragma once

#include "Model.h"
#include "Coordinator.h"
#include <unordered_map>

class GameObject
{
protected:
	std::vector<Model*> models;
	std::unordered_map<Model*, Coordinator*> coordinators;
	//std::vector<Coordinator*> coordinators;
	PhysicsObject* physics;

public:
	GameObject();

	GameObject(PhysicsObject* physics);

	PhysicsObject* GetPhysicsObject()
	{
		return physics;
	}

	/*void Delete()
	{
		for (Model* model : models)
		{
			delete model->GetMesh();
			delete model;
		}
		for (auto& pair : coordinators)
		{
			delete pair.second;
		}
		delete physics;
	}*/

	void AddModel(Model* model, Coordinator* coordinator)
	{
		models.push_back(model);
		coordinators[model] = coordinator;
	}

	std::vector<Model*> GetModels()
	{
		return models;
	}

	void Render(Shader& shader, const char* uniformName)
	{
		for(Model* model : models)
		{
			coordinators[model]->coordinate(model, physics);
			model->Render(shader, uniformName);
		}
	}

	void Update(float deltaTime)
	{
		physics->Update(deltaTime);
	}

	void Coordinate()
	{
		for (Model* model : models)
		{
			coordinators[model]->coordinate(model, physics);
		}
	}

	int GetType() const
	{
		if(physics != nullptr)
			return physics->GetType();
		return NO_PHYSICS;
	}

	void SetColor(const Color& color)
	{
		for (Model* model : models)
		{
			model->GetMesh()->SetColor(color);
		}
	}

	std::vector<Model*> GetModels() const
	{
		return models;
	}

	friend class Engine;
};

void DeleteGameObjects();