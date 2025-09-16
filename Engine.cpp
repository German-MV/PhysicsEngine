#include "Engine.h"

#include <iostream>

std::vector<GameObject*> addedModels;

Engine::Engine() : 
	physicsSystem(PhysicsSystem()), 
	scene(Scene())
{}

//Engine::~Engine()
//{
//	for (GameObject* object : addedModels)
//	{
//		delete[] object;
//	}
//	/*for (GameObject* object : objects)
//	{
//		for (Model* model : object->models)
//		{
//			scene->RemoveModel(model);
//			delete modelToObjectMap[model];
//			modelToObjectMap.erase(model);
//		}
//		delete object;
//	}*/
//}

void Engine::AddObject(GameObject* object)
{
	if (object == nullptr)
		return;

	if (std::find(objects.begin(), objects.end(), object) != objects.end())
	{
		std::cout << "Object already exists in the engine." << std::endl;
		return;
	}

	objects.push_back(object);

	if(object->physics != nullptr)
		physicsSystem.AddObject(object->physics);
	
	for (Model* model : object->models)
	{
		scene.AddModel(model);
		modelToObjectMap[model] = object;
	}
}

void Engine::AddModel(Model* model)
{
	if (model == nullptr)
		return;

	if (std::find(scene.objects.begin(), scene.objects.end(), model) != scene.objects.end())
	{
		std::cout << "Model already exists in the scene." << std::endl;
		return;
	}

	GameObject* object = new GameObject();
	addedModels.push_back(object);
	object->models.push_back(model);
	object->coordinators[model] = nullptr;
	object->physics = nullptr;

	AddObject(object);
}

void Engine::RemoveGameObject(GameObject* object)
{
	auto it = std::find(objects.begin(), objects.end(), object);
	if (it != objects.end())
	{
		objects.erase(it);

		for (Model* model : object->models)
		{
			scene.RemoveModel(model);
			modelToObjectMap.erase(model);
		}

		physicsSystem.RemoveObject(object->physics);
		//delete object;
	}
}

void Engine::Update(float deltaTime)
{
	physicsSystem.Update(deltaTime);
}

void Engine::Coordinate()
{
	for (GameObject* object : objects)
	{
		for (Model* model : object->models)
		{
			if (object->coordinators[model] != nullptr)
				object->coordinators[model]->coordinate(model, object->physics);
		}
	}
}


//#include "GeometrySamples.h"
void Engine::Render(const Frustum& frustum, Shader& shader, const char* uniformName)
{
	Coordinate();

	std::vector<Model*> modelsInFrustum = scene.Cull(frustum);

	for (Model* model : modelsInFrustum)
	{
		//DrawOBB(GetOBB(*model), shader, uniformName);
		model->Render(shader, uniformName);
	}
}

GameSelection Engine::Raycast(const Ray& ray)
{
	Model* model = scene.Raycast(ray);
	//std::cout << "Raycasted model: " << model << std::endl;
	if (model == nullptr) return NullSelection;
	//std::cout << "Selection type: " << modelToObjectMap[model]->GetType() << std::endl;
	float t = ::Raycast(*model, ray);
	if (t < 0.f) return NullSelection;

	glm::vec3 point = ray.origin + ray.direction * t;
	GameObject* object = modelToObjectMap[model];

	return GameSelection(object, point);
}