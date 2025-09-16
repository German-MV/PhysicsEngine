#include "Model.h"

//void Model::SetPosition(const glm::vec3& position)
//{
//	this->position = position;
//	bounds.position = position;
//}
//
//void Model::SetScale(const glm::vec3& scale)
//{
//	this->scale = scale;
//	bounds.size = scale;
//}
//
//void Model::SetOrientation(const glm::quat& orientation)
//{
//	this->orientation = orientation;
//}

void Model::SetContent(Mesh* mesh)
{
	content = mesh;
	if (content != 0)
	{
		glm::vec3 min = mesh->vertices[0].position;
		glm::vec3 max = mesh->vertices[0].position;

		for (int i = 1; i < mesh->GetNumVertices(); ++i)
		{
			min.x = fminf(mesh->vertices[i].position.x, min.x);
			min.y = fminf(mesh->vertices[i].position.y, min.y);
			min.z = fminf(mesh->vertices[i].position.z, min.z);

			max.x = fmaxf(mesh->vertices[i].position.x, max.x);
			max.y = fmaxf(mesh->vertices[i].position.y, max.y);
			max.z = fmaxf(mesh->vertices[i].position.z, max.z);
		}

		bounds = FromMinMax(min, max);
	}
}

#include <iostream>
void Model::Render(Shader& shader, const char* uniformName) const
{
	if (content == 0) {
		std::cout << "Model has no content" << std::endl;
		return;
	}

	//shader.Activate();

	shader.SetUniformMat4(GetWorldMatrix(), uniformName);

	content->Render();
}

glm::mat4 Model::GetWorldMatrix() const
{
	glm::mat4 scale = glm::scale(glm::mat4(1.f), this->scale);
	glm::mat4 rotation = glm::mat4_cast(this->orientation);
	glm::mat4 translation = glm::translate(glm::mat4(1.f), this->position);

	glm::mat4 localMat = translation * rotation * scale;

	if (parent == 0) return localMat;

	glm::mat4 parentMat = parent->GetWorldMatrix();

	return parentMat * localMat;
}

glm::mat4 GetWorldMatrix(const Model& model)
{
	return model.GetWorldMatrix();
}

OBB GetOBB(const Model& model)
{
	glm::mat4 world = GetWorldMatrix(model);

	AABB aabb = model.GetBounds();

	OBB obb;

	obb.size = aabb.size * model.scale;
	obb.position = MultiplyPoint(aabb.position, world);
	obb.orientation = glm::mat3(model.orientation);

	return obb;
}


float Raycast(const Model& model, const Ray& ray)
{
	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	Ray local;
	local.origin = MultiplyPoint(ray.origin, inv);
	local.direction = MultiplyVector(ray.direction, inv);
	local.NormalizeDirection();

	if (model.GetMesh() != 0)
	{
		float tLocal = Raycast(*(model.GetMesh()), local);
		//float t = glm::length(MultiplyVector(local.direction * tLocal, world));
		//float t = glm::length(MultiplyVector(local.direction * tLocal, world) - ray.origin);
		float t = tLocal * glm::length(local.direction * model.scale);
		return t;
	}

	return -1.f;
}

bool Linetest(const Model& model, const Line& line)
{
	if (model.GetMesh() == 0) return false;

	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	Line local;

	local.start = MultiplyPoint(line.start, inv);
	local.end = MultiplyPoint(line.end, inv);

	return Linetest(*(model.GetMesh()), local);
}

bool ModelSphere(const Model& model, const Sphere& sphere)
{
	if (model.GetMesh() == 0) return false;

	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	Sphere local;
	local.position = MultiplyPoint(sphere.position, inv);

	return MeshSphere(*(model.GetMesh()), local);
}

bool ModelAABB(const Model& model, const AABB& aabb)
{
	if (model.GetMesh() == 0) return false;

	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	OBB local;
	local.size = aabb.size;
	local.position = MultiplyPoint(aabb.position, inv);
	local.orientation = glm::mat3(inv);

	return MeshOBB(*(model.GetMesh()), local);
}

bool ModelOBB(const Model& model, const OBB& obb)
{
	if (model.GetMesh() == 0) return false;

	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	OBB local;
	local.size = obb.size;
	local.position = MultiplyPoint(obb.position, inv);
	local.orientation = obb.orientation * glm::mat3(inv);

	return MeshOBB(*(model.GetMesh()), local);
}

bool ModelPlane(const Model& model, const Plane& plane)
{
	if (model.GetMesh() == 0) return false;

	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	Plane local;
	local.normal = MultiplyVector(plane.normal, inv);
	local.distance = plane.distance; // mis dudas

	return MeshPlane(*(model.GetMesh()), local);
}

bool ModelTriangle(const Model& model, const Triangle& triangle)
{
	if (model.GetMesh() == 0) return false;

	glm::mat4 world = GetWorldMatrix(model);
	glm::mat4 inv = glm::inverse(world);

	Triangle local;
	local.a = MultiplyPoint(triangle.a, inv);
	local.b = MultiplyPoint(triangle.b, inv);
	local.c = MultiplyPoint(triangle.c, inv);

	return MeshTriangle(*(model.GetMesh()), local);
}