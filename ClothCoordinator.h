#pragma once
#include "Coordinator.h"
#include "Cloth.h"

class ClothCoordinator : public Coordinator
{
public:
	void coordinate(Model* model, PhysicsObject* object) const
	{
		Cloth* cloth = ToCloth(object);
		if (!cloth) return;

		Mesh* mesh = model->GetMesh();

		int numVerticesFace1 = mesh->vertices.size() / 2;

		//mesh->vertices.clear();

		int height = cloth->GetHeight();
		int width = cloth->GetWidth();

		for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			mesh->vertices[x + y * width].position = cloth->GetParticle(x, y).GetPosition();
			mesh->vertices[x + y * width].normal = glm::vec3(0.f);

			mesh->vertices[numVerticesFace1 + x + y * width].position = cloth->GetParticle(x, y).GetPosition();
			mesh->vertices[numVerticesFace1 + x + y * width].normal = glm::vec3(0.f);
		}

		// Calculate the normals
		for (int i = 0; i < mesh->GetNumTriangles(); ++i)
		{
			Triangle t = mesh->GetTriangle(i);
			glm::vec3 normal = -glm::normalize(glm::cross(t.b - t.a, t.c - t.a));

			mesh->vertices[mesh->indices[i * 3 + 0]].normal += normal;
			mesh->vertices[mesh->indices[i * 3 + 1]].normal += normal;
			mesh->vertices[mesh->indices[i * 3 + 2]].normal += normal;
		}

		// Normalize the normals
		for (int i = 0; i < mesh->GetNumVertices(); ++i)
		{
			mesh->vertices[i].normal = glm::normalize(mesh->vertices[i].normal);
		}

		// apply the changes
		mesh->Update();

		model->SetContent(mesh);
	}
};

