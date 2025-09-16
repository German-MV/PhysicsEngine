#pragma once

// Basado en el libro Szauer 2017

#include "Geometry3D.h"
#include "Model.h"

#include <vector>

struct OctreeNode
{
	AABB bounds;
	OctreeNode* children;
	std::vector<Model*> models;

	inline OctreeNode() : children(0) {}

	inline ~OctreeNode()
	{
		if (children != 0)
			delete[] children;
	}
};

class Scene {
//protected:
	//private:
		//Scene(const Scene&);

		//Scene& operator = (const Scene&); // Aparece en el libro Szauer 2017 pero nunca lo definen
public:
	OctreeNode* octree;

	std::vector<Model*> objects;

	inline Scene() : octree(0) {}

	inline ~Scene()
	{
		if (octree != 0)
			delete octree;
	}

	void AddModel(Model* model);

	void RemoveModel(Model* model);

	//void UpdateModel(Model* model);

	std::vector<Model*> FindChildren(const Model* model);

	Model* Raycast(const Ray& ray);

	std::vector<Model*> Query(const Sphere& sphere);

	std::vector<Model*> Query(const AABB& aabb);

	bool Accelerate(const glm::vec3& position, float size);

	std::vector<Model*> Cull(const Frustum& f);
};

void SplitTree(OctreeNode* node, int depth);
// dado un nodo con o sin hijos, se encarga de determinar que objetos contiene cada uno de los hijos (de no haberlos los crea) y elimina la lista de modelos original (ya insertada en los hijos)
// de esta manera queda una red de cajas que contienen los punteros a los modelos dentro de sus bordes.

void Insert(OctreeNode* node, Model* model);

void Remove(OctreeNode* node, Model* model);

void Update(OctreeNode* node, Model* model);

Model* FindClosest(const std::vector<Model*>& set, const Ray& ray);

Model* Raycast(OctreeNode* node, const Ray& ray);

std::vector<Model*> Query(OctreeNode* node, const Sphere& sphere);

std::vector<Model*> Query(OctreeNode* node, const AABB& aabb);