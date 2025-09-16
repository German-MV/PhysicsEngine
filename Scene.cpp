#include "Scene.h"
#include <algorithm>
#include <stack>

void Scene::AddModel(Model* model)
{
	if (std::find(objects.begin(), objects.end(), model) != objects.end())
		return; // El objeto ya estaba en la escena

	objects.push_back(model);
}

void Scene::RemoveModel(Model* model)
{
	objects.erase(std::remove(objects.begin(), objects.end(), model), objects.end());
}

//void Scene::UpdateModel(Model* model)
//{
//
//}

std::vector<Model*> Scene::FindChildren(const Model* model)
{
	std::vector<Model*> result;

	for (Model* &object : objects) // Algoritmo propuesto por Szauer 2017
	{
		if (object == 0 || object == model)
			continue;

		Model* iterator = object->parent;
		if (iterator != 0)
		{
			if (iterator == model)
			{
				result.push_back(object);
				continue;
			}
			iterator = iterator->parent;
		}
	}

	/*for (Model*& object : objects)
	{
		std::vector<Model*> children = model->children;

		if (std::find(children.begin(), children.end(), object) != children.end())
		{
			result.push_back(object);
		}
	}*/

	return result;
}

Model* Scene::Raycast(const Ray& ray)
{
	if (octree != 0)
		return ::Raycast(octree, ray);

	Model* result = 0;
	float tResult = -1.f;
	for (Model*& object : objects)
	{
		float t = ::Raycast(*object, ray);

		if (t < 0.f) continue; // linea anadida por mi

		if (result == 0)
		{
			result = object;
			tResult = t;
		}
		else if (t < tResult)
		{
			result = object;
			tResult = t;
		}
	}

	return result;
}

std::vector<Model*> Scene::Query(const Sphere& sphere)
{
	if (octree != 0)
		return ::Query(octree, sphere);

	std::vector<Model*> result;

	for (Model*& object : objects)
	{
		OBB bounds = GetOBB(*object);

		if (SphereOBB(sphere, bounds))
			result.push_back(object);
	}

	return result;
}

std::vector<Model*> Scene::Query(const AABB& aabb)
{
	if (octree != 0)
		return ::Query(octree, aabb);

	std::vector<Model*> result;

	for (Model*& object : objects)
	{
		OBB bounds = GetOBB(*object);

		if (AABBOBB(aabb, bounds))
			result.push_back(object);
	}

	return result;
}

void SplitTree(OctreeNode* node, int depth) 
{
	if (depth-- <= 0) return;

	if (node->children == 0) // Si no tiene hijos anade nuevos hijos
	{
		node->children = new OctreeNode[8];

		// Crea 8 cajas.
		glm::vec3 c = node->bounds.position;
		glm::vec3 e = node->bounds.size * 0.5f;

		node->children[0].bounds = AABB(c + glm::vec3(-e.x, +e.y, -e.z), e);
		node->children[1].bounds = AABB(c + glm::vec3(+e.x, +e.y, -e.z), e);
		node->children[2].bounds = AABB(c + glm::vec3(-e.x, +e.y, +e.z), e);
		node->children[3].bounds = AABB(c + glm::vec3(+e.x, +e.y, +e.z), e);
		node->children[4].bounds = AABB(c + glm::vec3(-e.x, -e.y, -e.z), e);
		node->children[5].bounds = AABB(c + glm::vec3(+e.x, -e.y, -e.z), e);
		node->children[6].bounds = AABB(c + glm::vec3(-e.x, -e.y, +e.z), e);
		node->children[7].bounds = AABB(c + glm::vec3(+e.x, -e.y, +e.z), e);
	}

	if (node->models.size() <= 0) return;

	for (int i = 0; i < 8; ++i)
	{
		for (Model*& model : node->models)
		{
			OBB bounds = GetOBB(*model);
			if (AABBOBB(node->children[i].bounds, bounds))
			{
				node->children[i].models.push_back(model);
			}
		}
	}

	node->models.clear();

	for (int i = 0; i < 8; ++i)
		SplitTree(&(node->children[i]), depth);
}

//#include <list>
//void Insert(OctreeNode* node, Model* model) // NON RECURSIVE SOLUTION (hecho por mi)
//{
//	OBB bounds = GetOBB(*model);
//	
//	if (!OBBAABB(bounds, node->bounds)) return;
//
//	std::list<OctreeNode*> toCheck;
//
//	toCheck.push_front(node);
//
//	while (!toCheck.empty())
//	{
//		OctreeNode* iterator = *(toCheck.begin());
//		toCheck.erase(toCheck.begin());
//
//		for (int i = 0; i < 8; ++i)
//		{
//			OctreeNode* child = &iterator->children[i];
//
//			if (OBBAABB(bounds, child->bounds))
//			{
//				if (child->children == 0)
//					child->models.push_back(model);
//
//				else toCheck.push_front(&iterator->children[i]);
//			}
//		}
//	}
//}

void Insert(OctreeNode* node, Model* model)
{
	OBB bounds = GetOBB(*model);

	if (!AABBOBB(node->bounds, bounds)) return;

	if (node->children == 0)
	{
		node->models.push_back(model);
		return;
	}

	for (int i = 0; i < 8; ++i)
		Insert(&(node->children[i]), model);
}

void Remove(OctreeNode* node, Model* model)
{
	if (node->children == 0)
	{
		std::vector<Model*>::iterator it = std::find(node->models.begin(), node->models.end(), model);

		if(it != node->models.end())
			node->models.erase(it);

		return;
	}

	for (int i = 0; i < 8; ++i)
		Remove(&(node->children[i]), model);
}

void Update(OctreeNode* node, Model* model)
{
	Remove(node, model);
	Insert(node, model);
}

Model* FindClosest(const std::vector<Model*>& set, const Ray& ray)
{
	float tClosest = -1.f;
	Model* closest = 0;

	for (Model* const& object : set)
	{
		float t = Raycast(*object, ray);

		if (t < 0.f) continue;

		if (tClosest < 0.f || t < tClosest)
		{
			tClosest = t;
			closest = object;
		}
	}

	return closest;
}

Model* Raycast(OctreeNode* node, const Ray& ray)
{
	if (Raycast(node->bounds, ray) < 0.f) return 0;

	if (node->children == 0)
		return FindClosest(node->models, ray);

	std::vector<Model*> results;

	for (int i = 0; i < 8; ++i)
	{
		Model* result = Raycast(&(node->children[i]), ray);
		if (result == 0) continue;
		results.push_back(result);
	}

	return FindClosest(results, ray);
}

std::vector<Model*> Query(OctreeNode* node, const Sphere& sphere)
{
	if (!AABBSphere(node->bounds, sphere)) 
		return std::vector<Model*>();

	std::vector<Model*> result;

	if (node->children != 0)
	{
		for (int i = 0; i < 8; ++i)
		{
			std::vector<Model*> childResult= Query(&(node->children[i]), sphere);

			result.insert(result.end(), childResult.begin(), childResult.end());
		}

		return result;
	}

	for (Model*& object : node->models)
	{
		OBB bounds = GetOBB(*object);

		if (OBBSphere(bounds, sphere))
			result.push_back(object);
	}

	return result;
}

std::vector<Model*> Query(OctreeNode* node, const AABB& aabb)
{
	if (!AABBAABB(node->bounds, aabb))
		return std::vector<Model*>();

	std::vector<Model*> result;

	if (node->children != 0)
	{
		for (int i = 0; i < 8; ++i)
		{
			std::vector<Model*> childResult = Query(&(node->children[i]), aabb);

			result.insert(result.end(), childResult.begin(), childResult.end());
		}

		return result;
	}

	for (Model*& object : node->models)
	{
		OBB bounds = GetOBB(*object);

		if (OBBAABB(bounds, aabb))
			result.push_back(object);
	}

	return result;
}

bool Scene::Accelerate(const glm::vec3& position, float size)
{
	if (octree != 0) return false;

	glm::vec3 min(
		position.x - size,
		position.y - size,
		position.z - size
	);

	glm::vec3 max(
		position.x + size,
		position.y + size,
		position.z + size
	);

	octree = new OctreeNode();
	octree->bounds = FromMinMax(min, max);
	octree->children = 0;

	for (Model*& object : objects)
		octree->models.push_back(object);

	SplitTree(octree, 5);
	return true;
}

#include <list>
std::vector<Model*> Scene::Cull(const Frustum& f)
{
	std::vector<Model*> result;

	if (octree == 0)
	{
		for (Model* object : objects)
		{
			OBB bounds = GetOBB(*object);
			if (Intersects(f, bounds))
				result.push_back(object);
		}

		return result;
	}
	// Then octree != 0
	std::list<OctreeNode*> nodes;
	nodes.push_back(octree);

	while (!nodes.empty())
	{
		OctreeNode* iterator = *nodes.begin();
		nodes.pop_front();

		if (iterator->children == 0)
		{
			for (Model* object : iterator->models)
			{
				OBB bounds = GetOBB(*object);
				if (Intersects(f, bounds))
					result.push_back(object);
			}
		}
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				OctreeNode* child = &iterator->children[i];
				if(Intersects(f, child->bounds))
					nodes.push_back(&iterator->children[i]);
			}
		}
	}

	return result;
}