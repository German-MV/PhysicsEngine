#pragma once

#include "GameObject.h"
#include "GameRigidBody.h"
//#include "GameParticle.h"
//#include "GameCloth.h"

GameRigidBody CreateGameCube(float mass, float size);

GameRigidBody CreateGameBox(float mass, glm::vec3 size);

GameRigidBody CreateGameSphere(float mass, float radius);

GameRigidBody CreateGameEllipsoid(float mass, glm::vec3 size);

GameRigidBody CreateGameCylinder(float mass, float radius, float height);

//GameRigidBody CreateGameVoidCylinder(float mass, float innerRadius, float outerRadius, float height);

GameRigidBody CreateGameCone(float mass, float radius, float height);