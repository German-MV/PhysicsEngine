#include "Mesh.h"
#include <list>


Mesh::Mesh(
	const std::vector<Vertex>& vertices,
	const std::vector<GLuint>& indices,
	GLenum usage,
	bool accelerate
) : vertices(vertices), indices(indices)
{
	CreateBuffers(usage);
	accelerator = 0;
	if(accelerate) Accelerate();
}

void Mesh::SetVertices(std::vector<Vertex> vertices)
{
	this->vertices = vertices;
}

void Mesh::SetIndices(std::vector<GLuint> indices)
{
	this->indices = indices;
}

void Mesh::CreateBuffers(GLenum usage)
{
	vao.Bind();

	vbo = VBO(vertices, usage);
	EBO ebo(indices);

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	vao.LinkAttrib(vbo, 2, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}

void Mesh::SetColor(const Color& color)
{
	for (int i = 0; i < vertices.size(); ++i)
		vertices[i].color = color;

	vbo.Update(vertices);
}

void Mesh::SetVertexOrigin(const glm::vec3& newOrigin)
{
	for (int i = 0; i < vertices.size(); ++i)
		vertices[i].position -= newOrigin;

	vbo.Update(vertices);

}

void Mesh::Render()
{
	vao.Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	vao.Unbind();
}

void Mesh::Update()
{
	vbo.Update(vertices);
	if (accelerator != 0) {
		FreeBVHNode(accelerator);
		delete accelerator;
		accelerator = 0;
		Accelerate();
	}
}

void Mesh::Accelerate() {
	if (accelerator != 0) return;

	glm::vec3 min = vertices[0].position;
	glm::vec3 max = vertices[0].position;

	for (int i = 1; i < GetNumVertices(); ++i)
	{
		min.x = fminf(vertices[i].position.x, min.x);
		min.y = fminf(vertices[i].position.y, min.y);
		min.z = fminf(vertices[i].position.z, min.z);

		max.x = fmaxf(vertices[i].position.x, max.x);
		max.y = fmaxf(vertices[i].position.y, max.y);
		max.z = fmaxf(vertices[i].position.z, max.z);
	}

	accelerator = new BVHNode();
	accelerator->bounds = FromMinMax(min, max);
	accelerator->numTriangles = GetNumTriangles();

	accelerator->triangles = new int[GetNumTriangles()];

	for (int i = 0; i < GetNumTriangles(); ++i)
		accelerator->triangles[i] = i;

	SplitBVHNode(accelerator, *this, 3);
}

void Mesh::UpdateAccelerator()
{
	if (accelerator == 0) return;
	FreeBVHNode(accelerator);
	delete accelerator;
	accelerator = 0;
	Accelerate();
}

void AccelerateMesh(Mesh& mesh)
{
	mesh.Accelerate();
}

void SplitBVHNode(BVHNode* node, const Mesh& mesh, int depth)
{
	if (depth-- == 0) return;

	if (node->numTriangles <= 0) return; // Si no hay triangulos no tiene sentido hacer nada

	if (node->children == 0) // Si no tiene hijos anade nuevos hijos
	{
		node->children = new BVHNode[8];

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

	//if (node->children == 0) return; // Si no tienes hijos entonces no se puede hacer nada (aunque si no tenia hijos ya le hemos asignado 8 hijos, por lo que se puede omitir)

	// A continuacion asignamos a cada hijo una lista de triangulos.
	for (int i = 0; i < 8; ++i)
	{
		node->children[i].numTriangles = 0;

		for (int j = 0; j < node->numTriangles; ++j)
		{
			Triangle triangle = mesh.GetTriangle(node->triangles[j]);

			if (TriangleAABB(triangle, node->children[i].bounds)) // si el triangulo esta contenido en el AABB del hijo entonces el hijo tendra un triangulo mas
				node->children[i].numTriangles += 1;
		}

		if (node->children[i].numTriangles == 0) continue; // si no hay triangulos entonces pasar al siguiente hijo

		node->children[i].triangles = new int[node->children[i].numTriangles];

		int index = 0;

		for (int j = 0; j < node->numTriangles; ++j) {
			Triangle triangle = mesh.GetTriangle(node->triangles[j]);

			if (TriangleAABB(triangle, node->children[i].bounds)) // si el triangulo esta contenido en el AABB del hijo entonces se incluye
			{
				node->children[i].triangles[index++] = node->triangles[j];
			}
		}
	}

	// Limpiamos el nodo actual porque toda su informacion esta contenida en los hijos
	node->numTriangles = 0;
	delete[] node->triangles;
	node->triangles = 0;

	// Repetimos el proceso con cada uno de los hijos
	for (int i = 0; i < 8; ++i)
		SplitBVHNode(&node->children[i], mesh, depth);
}

void FreeBVHNode(BVHNode* node)
{
	if (node->children != 0)
	{
		for (int i = 0; i < 8; ++i)
			FreeBVHNode(&node->children[i]);

		delete[] node->children;
		node->children = 0;
	}

	if (node->numTriangles != 0 || node->triangles != 0)
	{
		delete[] node->triangles;
		node->triangles = 0;
		node->numTriangles = 0;
	}
}

float Raycast(const Mesh& mesh, const Ray& ray) // POSIBLE ERROR: No se considera el caso en el que el rayo da a dos triangulos, sino que se devolvera la distancia contra el primero que compruebe. Habria que tomar la menor de todas las distancias
{

	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
		{
			float result = Raycast(mesh.GetTriangle(i), ray);
			if (result >= 0) return result;
		}

		return -1.f;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	float rMin = -1.f;

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
			{
				float r = Raycast(mesh.GetTriangle(iterator->triangles[i]), ray);

				if (r < 0) continue;
				
				if (rMin < 0 || r < rMin) rMin = r;
			}
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (Raycast(iterator->children[i].bounds, ray) >= 0)
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return rMin;
}

float Raycast(const Mesh& mesh, const Ray& ray, int& triangle) // POSIBLE ERROR: No se considera el caso en el que el rayo da a dos triangulos, sino que se devolvera la distancia contra el primero que compruebe. Habria que tomar la menor de todas las distancias
{
	triangle = -1;

	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
		{
			float result = Raycast(mesh.GetTriangle(i), ray);
			if (result >= 0) 
			{
				triangle = i;
				return result;
			}
		}

		return -1.f;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	float rMin = -1.f;

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
			{
				float r = Raycast(mesh.GetTriangle(iterator->triangles[i]), ray);

				if (r >= 0) {
					if (rMin < 0 || r < rMin) 
					{
						rMin = r;
						triangle = iterator->triangles[i];
					}
				}
			}
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (Raycast(iterator->children[i].bounds, ray) >= 0)
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return rMin;
}

bool MeshAABB(const Mesh& mesh, const AABB& aabb)
{
	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
			if (TriangleAABB(mesh.GetTriangle(i), aabb)) return true;

		return false;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
				if (TriangleAABB(mesh.GetTriangle(iterator->triangles[i]), aabb)) return true;
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (AABBAABB(iterator->children[i].bounds, aabb))
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return false;
}

bool Linetest(const Mesh& mesh, const Line& line)
{
	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
			if (Linetest(mesh.GetTriangle(i), line)) return true;

		return false;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
				if (Linetest(mesh.GetTriangle(iterator->triangles[i]), line)) return true;
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (Linetest(iterator->children[i].bounds, line))
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return false;
}

bool MeshSphere(const Mesh& mesh, const Sphere& sphere)
{
	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
			if (TriangleSphere(mesh.GetTriangle(i), sphere)) return true;

		return false;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
				if (TriangleSphere(mesh.GetTriangle(iterator->triangles[i]), sphere)) return true;
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (AABBSphere(iterator->children[i].bounds, sphere))
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return false;
}

bool MeshOBB(const Mesh& mesh, const OBB& obb)
{
	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
			if (TriangleOBB(mesh.GetTriangle(i), obb)) return true;

		return false;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
				if (TriangleOBB(mesh.GetTriangle(iterator->triangles[i]), obb)) return true;
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (AABBOBB(iterator->children[i].bounds, obb))
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return false;
}

bool MeshPlane(const Mesh& mesh, const Plane& plane)
{
	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
			if (TrianglePlane(mesh.GetTriangle(i), plane)) return true;

		return false;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
				if (TrianglePlane(mesh.GetTriangle(iterator->triangles[i]), plane)) return true;
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (AABBPlane(iterator->children[i].bounds, plane))
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return false;
}

bool MeshTriangle(const Mesh& mesh, const Triangle& triangle)
{
	if (mesh.accelerator == 0)
	{
		for (int i = 0; i < mesh.GetNumTriangles(); ++i)
			if (TriangleTriangle(mesh.GetTriangle(i), triangle)) return true;

		return false;
	}

	// Entonces tiene acelerador por lo que no hace falta comprobarlo
	std::list<BVHNode*> toProcess;
	toProcess.push_front(mesh.accelerator);

	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		if (iterator->numTriangles >= 0)
		{
			for (int i = 0; i < iterator->numTriangles; ++i)
				if (TriangleTriangle(mesh.GetTriangle(iterator->triangles[i]), triangle)) return true;
		}

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				if (AABBTriangle(iterator->children[i].bounds, triangle))
					toProcess.push_front(&iterator->children[i]);
			}
		}
	}

	return false;
}
