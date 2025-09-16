#pragma once

#include "SimpleGeometry.h"
#include "Colors.h"
#include "VAO.h"
#include "EBO.h"

typedef struct BVHNode
{
	AABB bounds;
	BVHNode* children;
	int numTriangles;
	int* triangles;
	BVHNode() : children(0), numTriangles(0), triangles(0) {}
} BVHNode;

//struct Mesh
//{
//	int numTriangles;
//
//	union
//	{
//		Triangle* triangles;
//		Point* vertices;
//		float* values;
//	};
//
//	BVHNode* accelerator;
//
//	Mesh() : numTriangles(0), values(0), accelerator(0) {}
//};

//#include <glad/glad.h>


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	Color color;

	Vertex() : position(0.f), normal(0.f), color(1.f) {}
	Vertex(const glm::vec3& position, const glm::vec3& normal, const Color& color) :
		position(position), normal(normal), color(color) {}
};

//template<typename Vertex>
class Mesh
{
public:
	VAO vao;
	VBO vbo;

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	BVHNode* accelerator;

	Mesh() : accelerator(0) {}

	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		GLenum usage = GL_STATIC_DRAW,
		bool accelerate = true
	);

	void SetVertices(std::vector<Vertex> vertices);

	void SetIndices(std::vector<GLuint> indices);

	void CreateBuffers(GLenum usage = GL_STATIC_DRAW);

	void SetColor(const Color& color);

	inline unsigned int GetNumVertices() const { return vertices.size(); }

	inline unsigned int GetNumIndices() const { return indices.size(); }

	inline unsigned int GetNumTriangles() const { return indices.size() / 3; }

	void SetVertexOrigin(const glm::vec3& newOrigin);

	Triangle GetTriangle(int index) const {
		Triangle t;
		t.a = vertices[indices[index * 3 + 0]].position;
		t.b = vertices[indices[index * 3 + 1]].position;
		t.c = vertices[indices[index * 3 + 2]].position;
		return t;
	}

	void Render();

	void Update();

	void Accelerate();

	void UpdateAccelerator();
};

void AccelerateMesh(Mesh& mesh);

void SplitBVHNode(BVHNode* node, const Mesh& model, int depth);

void FreeBVHNode(BVHNode* node);

float Raycast(const Mesh& mesh, const Ray& ray);

//int GetRaycastedTriangle(const Mesh& mesh, const Ray& ray);

bool MeshAABB(const Mesh& mesh, const AABB& aabb);

#define AABBMesh(aabb, mesh) MeshAABB(mesh, aabb)

bool Linetest(const Mesh& mesh, const Line& line);

bool MeshSphere(const Mesh& mesh, const Sphere& sphere);

#define SphereMesh(sphere, mesh) MeshSphere(mesh, sphere)

bool MeshOBB(const Mesh& mesh, const OBB& obb);

#define OBBMesh(obb, mesh) MeshOBB(mesh, obb)

bool MeshPlane(const Mesh& mesh, const Plane& plane);

#define PlaneMesh(plane, mesh) MeshPlane(mesh, plane)

bool MeshTriangle(const Mesh& mesh, const Triangle& triangle);

#define TriangleMesh(triangle, mesh) MeshTriangle(mesh, triangle)