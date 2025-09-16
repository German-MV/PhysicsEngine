#pragma once

#include "PhysicsSystem.h"
#include "Scene.h"
#include "GameObject.h"
#include "GameSelection.h"
#include <unordered_map>

class Engine
{
private:
	std::unordered_map<Model*, GameObject*> modelToObjectMap;

public:
	PhysicsSystem physicsSystem;
	Scene scene;

	std::vector<GameObject*> objects;

	Engine();
	//~Engine();

	void AddObject(GameObject* object);
	void AddModel(Model* model);

	void RemoveGameObject(GameObject* object);

	void Update(float deltaTime);

	void Coordinate();

	void Render(const Frustum& frustum, Shader& shader, const char* uniformName);

	GameSelection Raycast(const Ray& ray);
};