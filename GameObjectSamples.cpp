#include "GameObjectSamples.h"

#include "GeometrySamples.h"
#include "RigidBodySamples.h"

GameRigidBody CreateGameCube(float mass, float size)
{
	GameRigidBody cube = CreateGameBox(mass, glm::vec3(size, size, size));

	return cube;
}

GameRigidBody CreateGameBox(float mass, glm::vec3 size)
{
	GameRigidBody box(CreateRigidBox(mass, size));

	Mesh* boxMesh = CreateCubeMesh(WHITE);
	Model* model = new Model(boxMesh);
	model->scale = size;

	box.AddModel(model);

	return box;
}

GameRigidBody CreateGameSphere(float mass, float radius)
{
	GameRigidBody sphere = CreateGameEllipsoid(mass, glm::vec3(radius));

	return sphere;
}

GameRigidBody CreateGameEllipsoid(float mass, glm::vec3 size)
{
	GameRigidBody ellipsoid(CreateRigidEllipsoid(mass, size));

	Mesh* ellipsoidMesh = CreateSphereMesh(20, 20, WHITE);
	Model* model = new Model(ellipsoidMesh);
	model->scale = size;

	ellipsoid.AddModel(model);

	return ellipsoid;
}

GameRigidBody CreateGameCylinder(float mass, float radius, float height)
{
	GameRigidBody cylinder(CreateRigidCylinder(mass, radius, height));

	Mesh* cylinderMesh = CreateCylinderMesh(20, WHITE);
	Model* model = new Model(cylinderMesh);
	model->scale = glm::vec3(radius, height, radius);

	cylinder.AddModel(model);

	return cylinder;
}

GameRigidBody CreateGameCone(float mass, float radius, float height)
{
	GameRigidBody cone(CreateRigidCone(mass, radius, height));

	Mesh* coneMesh = CreateConeMesh(20, WHITE);
	Model* model = new Model(coneMesh);
	model->scale = glm::vec3(radius, height, radius);

	// Set the position of the model to be at the center of mass of the cone
	model->position = glm::vec3(0.f, height / 4.f, 0.f);

	cone.AddModel(model);

	return cone;
}