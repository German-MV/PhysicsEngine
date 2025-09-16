#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

// Basado en el libro Szauer 2017

#define DEG2RAD(x) ((x) * 0.0174533f)

// POINT
typedef glm::vec3 Point;

glm::vec3 MultiplyPoint(const glm::vec3& point, const glm::mat4& matrix);

glm::vec3 MultiplyVector(const glm::vec3& point, const glm::mat4& matrix);

// LINE
struct Line
{
	Point start, end;

	inline Line() : start(Point(0.f)), end(Point(0.f)) {}

	inline Line(const Point& startPoint, const Point& endPoint) :
		start(startPoint), end(endPoint) {}
};

float Length(const Line& line);

float LengthSq(const Line& line);

// RAY
struct Ray
{
	Point origin;
	glm::vec3 direction;

	inline Ray() : origin(Point(0.f)), direction(0.f, 0.f, 1.f) {}

	inline Ray(const Point& origin, const glm::vec3& direction) :
		origin(origin), direction(direction)
	{
		NormalizeDirection();
	}

	inline void NormalizeDirection()
	{
		//direction = glm::normalize(direction);
		direction /= glm::length(direction); // Comprobar si es mas rapido
	}
};

Ray FromPoints(const Point& from, const Point& to);

struct RaycastResult
{
	glm::vec3 point;
	glm::vec3 normal;
	float t;
	bool hit;
};

void ResetRaycastResult(RaycastResult* outResult);

// SPHERE
struct Sphere
{
	Point position;
	float radius;

	inline Sphere() : position(Point(0.f)), radius(1.f) {}

	inline Sphere(const Point& position, float radius) :
		position(position), radius(radius) {}
};

// AXIS ALIGNED BOUNDING BOX
struct AABB 
{
	Point position;
	glm::vec3 size;

	inline AABB() : position(Point(0.f)), size(1.f, 1.f, 1.f) {}

	inline AABB(const Point& position, const glm::vec3& size) :
		position(position), size(size) {}
};

glm::vec3 GetMin(const AABB& aabb); // Devuelve la esquina con coordenadas x, y, z mas pequenas

glm::vec3 GetMax(const AABB& aabb); // Devuelve la esquina con coordenadas x, y, z mas grandes

AABB FromMinMax(const glm::vec3& min, const glm::vec3& max); // Devuelve un AABB con esquinas minima y maxima

// ORIENTED BOUNDING BOX
struct OBB
{
	Point position;
	glm::vec3 size;
	glm::mat3 orientation;

	inline OBB() : position(Point(0.f)), size(1.f, 1.f, 1.f), orientation(glm::mat3(1.f)) {}

	inline OBB(const Point& position, const glm::vec3& size) :
		position(position), size(size), orientation(glm::mat3(1.f)) {}

	inline OBB(const Point& position, const glm::vec3 size, const glm::mat3& orientation) :
		position(position), size(size), orientation(orientation) {}
};

// PLANE
struct Plane
{
	glm::vec3 normal;
	float distance;

	inline Plane() : normal(1.f, 0.f, 0.f), distance(0.f) {}
	
	inline Plane(const glm::vec3& normal, float distance) :
		normal(normal), distance(distance) {}
};

float PlaneEquation(const Point& point, const Plane& plane); // devuelve la distancia al plano (0 si esta contenido en el)

// TRIANGLE
struct Triangle
{
	union
	{
		struct
		{
			Point a;
			Point b;
			Point c;
		};

		Point points[3];

		float values[9];
	};

	inline Triangle() : a(Point(0.f)), b(Point(0.f)), c(Point(0.f)) {}

	inline Triangle(const Point& p1, const Point& p2, const Point& p3) :
		a(p1), b(p2), c(p3) {}
};

// POINT TESTS
// Point and Sphere
bool PointInSphere(const Point& point, const Sphere& sphere);

Point ClosestPoint(const Point& point, const Sphere& sphere);

// Point and AABB
bool PointInAABB(const Point& point, const AABB& aabb);

Point ClosestPoint(const Point& point, const AABB& aabb);

// Point and OBB
bool PointInOBB(const Point& point, const OBB& obb);

Point ClosestPoint(const Point& point, const OBB& obb);

// Point and Plane
bool PointOnPlane(const Point& point, const Plane& plane);

Point ClosestPoint(const Point& point, const Plane& plane);

// Point and Line
bool PointOnLine(const Point& point, const Line& line);

Point ClosestPoint(const Point& point, const Line& line);

// Point and Ray
bool PointOnRay(const Point& point, const Ray& ray);

Point ClosestPoint(const Point& point, const Ray& ray);


// SHAPE INTERSECTIONS

bool SphereSphere(const Sphere& s1, const Sphere& s2);

bool SphereAABB(const Sphere& sphere, const AABB& aabb);

#define AABBSphere(aabb, sphere) SphereAABB(sphere, aabb)

bool SphereOBB(const Sphere& sphere, const OBB& obb);

#define OBBSphere(obb, sphere) SphereOBB(sphere, obb)

bool SpherePlane(const Sphere& sphere, const Plane& plane);

#define PlaneSphere(plane, sphere) SpherePlane(sphere, plane)

bool AABBAABB(const AABB& aabb1, const AABB& aabb2);

bool AABBOBB(const AABB& aabb, const OBB& obb);

#define OBBAABB(obb, aabb) AABBOBB(aabb, obb)

bool AABBPlane(const AABB& aabb, const Plane& plane);

#define PlaneAABB(plane, aabb) AABBPlane(aabb, plane)

bool OBBOBB(const OBB& obb1, const OBB& obb2);

bool OBBPlane(const OBB& obb, const Plane& plane);

#define PlaneOBB(plane, obb) OBBPlane(obb, plane)

bool PlanePlane(const Plane& plane1, const Plane& plane2);

// LINE INTERSECTIONS

float Raycast(const Sphere& sphere, const Ray& ray);

bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult);

float Raycast(const AABB& aabb, const Ray& ray);

bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult);

float Raycast(const OBB& obb, const Ray& ray);

bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult);

float Raycast(const Plane& plane, const Ray& ray);

bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult);

bool Linetest(const Sphere& sphere, const Line& line);

bool Linetest(const AABB& aabb, const Line& line);

bool Linetest(const OBB& obb, const Line& line);

bool Linetest(const Plane& plane, const Line& line);

// TRIANGLES

bool PointInTriangle(const Point& point, const Triangle& triangle);

Plane FromTriangle(const Triangle& triangle);

Point ClosestPoint(const Point& point, const Triangle& triangle);

bool TriangleSphere(const Triangle& triangle, const Sphere& sphere);

#define SphereTriangle(sphere, triangle) TriangleSphere(triangle, sphere)

bool TriangleAABB(const Triangle& triangle, const AABB& aabb);

#define AABBTriangle(aabb, triangle) TriangleAABB(triangle, aabb)

bool TriangleOBB(const Triangle& triangle, const OBB& obb);

#define OBBTriangle(obb, triangle) TriangleOBB(triangle, obb)

bool TrianglePlane(const Triangle& triangle, const Plane& plane);

#define PlaneTriangle(plane, triangle) TrianglePlane(triangle, plane)

bool TriangleTriangle(const Triangle& triangle1, const Triangle& triangle2);

bool TriangleTriangleRobust(const Triangle& triangle1, const Triangle& triangle2);

glm::vec3 project(const glm::vec3& length, const glm::vec3& direction);

float Raycast(const Triangle& triangle, const Ray& ray);

bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult);

bool Linetest(const Triangle& triangle, const Line& line);


struct Frustum
{
	union {
		struct {
			Plane top;
			Plane bottom;
			Plane left;
			Plane right;
			Plane near;
			Plane far;
		};
		Plane planes[6];
	};
	inline Frustum() : top(Plane()), bottom(Plane()), left(Plane()), right(Plane()), near(Plane()), far(Plane()) {}
};

Point Intersection(const Plane& p1, const Plane& p2, const Plane& p3);

void GetCorners(const Frustum& frustum, glm::vec3* outCorners);

bool Intersects(const Frustum& f, const Point& p);

bool Intersects(const Frustum& f, const Sphere& s);

float Classify(const AABB& aabb, const Plane& plane);

float Classify(const OBB& obb, const Plane& plane);

bool Intersects(const Frustum& f, const AABB& aabb);

bool Intersects(const Frustum& f, const OBB& obb);

glm::vec3 Unproject(
	int xViewport, int yViewport,
	float zViewport,
	int width, int height,
	const glm::mat4& camMatrix
);

Ray GetPickRay(
	int xViewport, int yViewport,
	int width, int height,
	const glm::mat4& camMatrix
);

/*
//bool DoIntersect(const Point& point1, const Point& point2)		{ return PointPoint(point1, point2); }
bool DoIntersect(const Point& point, const Line& line)				{ return PointOnLine(point, line); }
bool DoIntersect(const Point& point, const Sphere& sphere)			{ return PointInSphere(point, sphere); }
bool DoIntersect(const Point& point, const AABB& aabb)				{ return PointInAABB(point, aabb); }
bool DoIntersect(const Point& point, const OBB& obb)				{ return PointInOBB(point, obb); }
bool DoIntersect(const Point& point, const Plane& plane)			{ return PointOnPlane(point, plane); }
//bool DoIntersect(const Point& point, const Triangle& triangle)	{ return PointOnTriangle(point, triangle); }
//bool DoIntersect(const Point& point, const Mesh& mesh)			{ return PointInMesh(point, mesh); }
//bool DoIntersect(const Point& point, const Model& model)			{ return PointInModel(point, model) }

bool DoIntersect(const Line& line, const Point& point)				{ return PointOnLine(point, line); }
//bool DoIntersect(const Line& line1, const Line& line2)			{ return Linetest(line1, line2); }
bool DoIntersect(const Line& line, const Sphere& sphere)			{ return Linetest(sphere, line); }
bool DoIntersect(const Line& line, const AABB& aabb)				{ return Linetest(aabb, line); }
bool DoIntersect(const Line& line, const OBB& obb)					{ return Linetest(obb, line); }
bool DoIntersect(const Line& line, const Plane& plane)				{ return Linetest(plane, line); }
bool DoIntersect(const Line& line, const Triangle& triangle)		{ return Linetest(triangle, line); }
bool DoIntersect(const Line& line, const Mesh& mesh)				{ return Linetest(mesh, line); }
bool DoIntersect(const Line& line, const Model& model)				{ return Linetest(model, line); }

bool DoIntersect(const Sphere& sphere, const Point& point)			{ return PointInSphere(point, sphere); }
bool DoIntersect(const Sphere& sphere, const Line& line)			{ return Linetest(sphere, line); }
bool DoIntersect(const Sphere& sphere1, const Sphere& sphere2)		{ return SphereSphere(sphere1, sphere2); }
bool DoIntersect(const Sphere& sphere, const AABB& aabb)			{ return SphereAABB(sphere, aabb); }
bool DoIntersect(const Sphere& sphere, const OBB& obb)				{ return SphereOBB(sphere, obb); }
bool DoIntersect(const Sphere& sphere, const Plane& plane)			{ return SpherePlane(sphere, plane); }
bool DoIntersect(const Sphere& sphere, const Triangle& triangle)	{ return SphereTriangle(sphere, triangle); }
bool DoIntersect(const Sphere& sphere, const Mesh& mesh)			{ return SphereMesh(sphere, mesh); }
bool DoIntersect(const Sphere& sphere, const Model& model)			{ return SphereModel(sphere, model); }

bool DoIntersect(const AABB& aabb, const Point& point)				{ return PointInAABB(point, aabb); }
bool DoIntersect(const AABB& aabb, const Line& line)				{ return Linetest(aabb, line); }
bool DoIntersect(const AABB& aabb, const Sphere& sphere)			{ return AABBSphere(aabb, sphere); }
bool DoIntersect(const AABB& aabb1, const AABB& aabb2)				{ return AABBAABB(aabb1, aabb2); }
bool DoIntersect(const AABB& aabb, const OBB& obb)					{ return AABBOBB(aabb, obb); }
bool DoIntersect(const AABB& aabb, const Plane& plane)				{ return AABBPlane(aabb, plane); }
bool DoIntersect(const AABB& aabb, const Triangle& triangle)		{ return AABBTriangle(aabb, triangle); }
bool DoIntersect(const AABB& aabb, const Mesh& mesh)				{ return AABBMesh(aabb, mesh); }
bool DoIntersect(const AABB& aabb, const Model& model)				{ return AABBModel(aabb, model); }


bool DoIntersect(const OBB& obb, const Point& point)				{ return PointInOBB(point, obb); }
bool DoIntersect(const OBB& obb, const Line& line)					{ return Linetest(obb, line); }
bool DoIntersect(const OBB& obb, const Sphere& sphere)				{ return OBBSphere(obb, sphere); }
bool DoIntersect(const OBB& obb, const AABB& aabb)					{ return OBBAABB(obb, aabb); }
bool DoIntersect(const OBB& obb1, const OBB& obb2)					{ return OBBOBB(obb1, obb2); }
bool DoIntersect(const OBB& obb, const Plane& plane)				{ return OBBPlane(obb, plane); }
bool DoIntersect(const OBB& obb, const Triangle& triangle)			{ return OBBTriangle(obb, triangle); }
bool DoIntersect(const OBB& obb, const Mesh& mesh)					{ return OBBMesh(obb, mesh); }
bool DoIntersect(const OBB& obb, const Model& model)				{ return OBBModel(obb, model); }

bool DoIntersect(const Plane& plane, const Point& point)			{ return PointOnPlane(point, plane); }
bool DoIntersect(const Plane& plane, const Line& line)				{ return Linetest(plane, line); }
bool DoIntersect(const Plane& plane, const Sphere& sphere)			{ return PlaneSphere(plane, sphere); }
bool DoIntersect(const Plane& plane, const AABB& aabb)				{ return PlaneAABB(plane, aabb); }
bool DoIntersect(const Plane& plane, const OBB& obb)				{ return PlaneOBB(plane, obb); }
bool DoIntersect(const Plane& plane1, const Plane& plane2)			{ return PlanePlane(plane1, plane2); }
bool DoIntersect(const Plane& plane, const Triangle& triangle)		{ return PlaneTriangle(plane, triangle); }
bool DoIntersect(const Plane& plane, const Mesh& mesh)				{ return PlaneMesh(plane, mesh); }
bool DoIntersect(const Plane& plane, const Model& model)			{ return PlaneModel(plane, model); }

//bool DoIntersect(const Triangle& triangle, const Point& point)			{ return PointOnTriangle(point, triangle); }
bool DoIntersect(const Triangle& triangle, const Line& line)				{ return Linetest(triangle, line); }
bool DoIntersect(const Triangle& triangle, const Sphere& sphere)			{ return TriangleSphere(triangle, sphere); }
bool DoIntersect(const Triangle& triangle, const AABB& aabb)				{ return TriangleAABB(triangle, aabb); }
bool DoIntersect(const Triangle& triangle, const OBB& obb)					{ return TriangleOBB(triangle, obb); }
bool DoIntersect(const Triangle& triangle, const Plane& plane)				{ return TrianglePlane(triangle, plane); }
bool DoIntersect(const Triangle& triangle1, const Triangle& triangle2)		{ return TriangleTriangleRobust(triangle1, triangle2); }
bool DoIntersect(const Triangle& triangle, const Mesh& mesh)				{ return TriangleMesh(triangle, mesh); }
bool DoIntersect(const Triangle& triangle, const Model& model)				{ return TriangleModel(triangle, model); }

//bool DoIntersect(const Mesh& mesh, const Point& point)			{ return PointInMesh(point, mesh); }
bool DoIntersect(const Mesh& mesh, const Line& line)				{ return Linetest(mesh, line); }
bool DoIntersect(const Mesh& mesh, const Sphere& sphere)			{ return MeshSphere(mesh, sphere); }
bool DoIntersect(const Mesh& mesh, const AABB& aabb)				{ return MeshAABB(mesh, aabb); }
bool DoIntersect(const Mesh& mesh, const OBB& obb)					{ return MeshOBB(mesh, obb); }
bool DoIntersect(const Mesh& mesh, const Plane& plane)				{ return MeshPlane(mesh, plane); }
bool DoIntersect(const Mesh& mesh, const Triangle& triangle)		{ return MeshTriangle(mesh, triangle); }
//bool DoIntersect(const Mesh& mesh1, const Mesh& mesh2)			{ return MeshMesh(mesh1, mesh2); }
//bool DoIntersect(const Mesh& mesh, const Model& model)			{ return MeshModel(mesh, model); }

//bool DoIntersect(const Model& model, const Point& point)			{ return PointInModel(point, model); }
bool DoIntersect(const Model& model, const Line& line)				{ return Linetest(model, line); }
bool DoIntersect(const Model& model, const Sphere& sphere)			{ return ModelSphere(model, sphere); }
bool DoIntersect(const Model& model, const AABB& aabb)				{ return ModelAABB(model, aabb); }
bool DoIntersect(const Model& model, const OBB& obb)				{ return ModelOBB(model, obb); }
bool DoIntersect(const Model& model, const Plane& plane)			{ return ModelPlane(model, plane); }
bool DoIntersect(const Model& model, const Triangle& triangle)		{ return ModelTriangle(model, triangle); }
//bool DoIntersect(const Model& model, const Mesh& mesh)			{ return ModelMesh(model, mesh); }
//bool DoIntersect(const Model& model1, const Model& model2)		{ return ModelModel(model1, model2); }
*/