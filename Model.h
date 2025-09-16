#pragma once

//#include "Geometry3D.h"
#include "Mesh.h"
#include "Shaders.h"

class Model
{
protected:
	Mesh* content;
	AABB bounds;
	//std::vector<Model*> children; // En el apartado de escenas el libro Szauer 2017 sugiere que es más óptimo tener una lista de hijos.


public:
	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;

	Model* parent;

	inline Model() : parent(0), content(0), orientation(glm::quat(glm::vec3(0.f))), position(Point(0.f)), scale(glm::vec3(1.f)) {}

	inline Model(Mesh* mesh) : parent(0), content(0), orientation(glm::quat(glm::vec3(0.f))), position(Point(0.f)), scale(glm::vec3(1.f)) { SetContent(mesh); }

	/*void SetPosition(const glm::vec3& position);
	inline glm::vec3 GetPosition() const { return position; }

	void SetOrientation(const glm::quat& orientation);
	inline glm::quat GetOrientation() const { return orientation; }

	void SetScale(const glm::vec3& scale);
	inline glm::vec3 GetScale() const { return scale; }*/

	inline Mesh* GetMesh() const { return content; }

	inline AABB GetBounds() const { return bounds; }

	void SetContent(Mesh* mesh); // Inicializa mesh y bounds explorando todos los vertices de la red para determinar cual es el AABB minimo que los contiene a todos.

	glm::mat4 GetWorldMatrix() const;

	void Render(Shader& shader, const char* uniformName) const;
};

glm::mat4 GetWorldMatrix(const Model& model); // A partir de la posicion y la orientacion del modelo calcula la matriz 4x4 que contiene dicha informacion.

OBB GetOBB(const Model& model); // Devuelve el OBB correspondiente al AABB del modelo (bounds) pero orientado.

float Raycast(const Model& model, const Ray& ray);

bool Linetest(const Model& model, const Line& line);

bool ModelSphere(const Model& model, const Sphere& sphere);
# define SphereModel(sphere, model) ModelSphere(model, sphere)

bool ModelAABB(const Model& model, const AABB& aabb);
# define AABBModel(aabb, model) ModelAABB(model, aabb)

bool ModelOBB(const Model& model, const OBB& obb);
# define OBBModel(obb, model) ModelOBB(model, obb)

bool ModelPlane(const Model& model, const Plane& plane);
# define PlaneModel(plane, model) ModelPlane(model, plane)

bool ModelTriangle(const Model& model, const Triangle& triangle);
# define TriangleModel(triangle, model) ModelTriangle(model, triangle)