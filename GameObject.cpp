#include "GameObject.h"

std::vector<GameObject*> loadedGameObjects;

GameObject::GameObject() : physics(nullptr)
{
	loadedGameObjects.push_back(this);
}

GameObject::GameObject(PhysicsObject* physics) : physics(physics)
{
	loadedGameObjects.push_back(this);
}

void DeleteGameObjects()
{
	for (GameObject* gameObject : loadedGameObjects)
	{
		//gameObject->Delete();
	}
	loadedGameObjects.clear();
}