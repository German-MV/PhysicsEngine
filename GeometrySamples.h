#pragma once

#include "Geometry3D.h"

Mesh* CreatePlaneMesh(Color color = WHITE);

Mesh* CreateCubeMesh(Color color = WHITE);

Mesh* CreateSphereMesh(int nStacks = 20, int nSlices = 20, Color color = WHITE);

Mesh* CreateCylinderMesh(int sectorCount, Color color = WHITE);

Mesh* CreateConeMesh(int sectorCount, Color color = WHITE);

Mesh* CreateClothMesh(int width, int height, Color color = WHITE);

void DeleteGeometrySamples();

void DrawOBB(const OBB& obb, Shader& shader, const char* uniformLocation, Color color = PURPLE + TRANSPARENCY(0.5f));

void DrawAABB(const AABB& aabb, Shader& shader, const char* uniformLocation, Color color = ORANGE + TRANSPARENCY(0.5f));

void DrawSphere(float radius, const glm::vec3& position, Shader& shader, const char* uniformLocation, Color color = GREEN);

void DrawPoint(const glm::vec3& point, Shader& shader, const char* uniformLocation, Color color = RED);

void DrawLine(const glm::vec3& start, const glm::vec3& end, float radius, Shader& shader, const char* uniformLocation, Color color = BLACK);

void DrawVector(const glm::vec3& start, const glm::vec3& end, Shader& shader, const char* uniformLocation, Color color = BLACK);

void DrawRay(const Ray& ray, Shader& shader, const char* uniformLocation, Color color = BLACK);

void DrawPlane(const Plane& plane, Shader& shader, const char* uniformLocation, Color color = BLUE + TRANSPARENCY(0.8f));