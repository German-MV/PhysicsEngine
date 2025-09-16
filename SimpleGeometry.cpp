#include "SimpleGeometry.h"

#define CMP(number1, number2) (fabsf((number1)-(number2)) < 1E-10)

glm::vec3 MultiplyPoint(const glm::vec3& point, const glm::mat4& matrix)
{
	return glm::vec3(matrix * glm::vec4(point, 1.f));
}

glm::vec3 MultiplyVector(const glm::vec3& point, const glm::mat4& matrix)
{
	return glm::vec3(matrix * glm::vec4(point, 0.f));
}

// LINE
float Length(const Line& line)
{
	return glm::length(line.start - line.end);
}

float LengthSq(const Line& line)
{
	return glm::length2(line.start - line.end);
}

// RAY
Ray FromPoints(const Point& from, const Point& to)
{
	return Ray(from, to - from);
}

// AXIS ALIGNED BOUNDING BOX
glm::vec3 GetMin(const AABB& aabb)
{
	glm::vec3 p1 = aabb.position + aabb.size;
	glm::vec3 p2 = aabb.position - aabb.size;

	return glm::vec3(
		fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z)
	);
}

glm::vec3 GetMax(const AABB& aabb)
{
	glm::vec3 p1 = aabb.position + aabb.size;
	glm::vec3 p2 = aabb.position - aabb.size;

	return glm::vec3(
		fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z)
	);
}

AABB FromMinMax(const glm::vec3& min, const glm::vec3& max)
{
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

// PLANE
float PlaneEquation(const Point& point, const Plane& plane)
{
	return glm::dot(point, plane.normal) - plane.distance;
}

// POINT TESTS

// Point and Sphere
bool PointInSphere(const Point& point, const Sphere& sphere)
{
	return glm::distance2(point, sphere.position) < sphere.radius * sphere.radius;
}

Point ClosestPoint(const Point& point, const Sphere& sphere)
{
	return glm::normalize(point - sphere.position) * sphere.radius + sphere.position;
}

// Point and AABB
bool PointInAABB(const Point& point, const AABB& aabb)
{
	Point min = GetMin(aabb);
	Point max = GetMax(aabb);

	if (point.x < min.x || point.y < min.y || point.z < min.z)
		return false;

	if (point.x > max.x || point.y > max.y || point.z > max.z)
		return false;

	return true;
}

Point ClosestPoint(const Point& point, const AABB& aabb)
{
	Point result = point;

	Point min = GetMin(aabb);
	Point max = GetMax(aabb);

	// Si sus coordenadas son menores que las de la esquina min entonces seguro esta fuera del AABB y se escogeran las coordenadas correspondientes de min.
	result.x = (result.x < min.x) ? min.x : result.x;
	result.y = (result.y < min.y) ? min.y : result.y;
	result.z = (result.z < min.z) ? min.z : result.z;

	// Si sus coordenadas son mayores que las de la esquina max entonces seguro esta fuera del AABB y se escogeran las coordenadas correspondientes de min.
	result.x = (result.x > max.x) ? max.x : result.x;
	result.y = (result.y > max.y) ? max.y : result.y;
	result.z = (result.z > max.z) ? max.z : result.z;

	// Las coordenadas dentro del aabb se quedan igual, y las que están fuera se escogen las de la esquina min o max.
	return result;
}

// Point and OBB
bool PointInOBB(const Point& point, const OBB& obb)
{
	glm::vec3 dir = point - obb.position;
	
	for (int i = 0; i < 3; ++i)
	{
		// Las columnas de la matriz son los ejes del obb
		glm::vec3 axis(
			obb.orientation[0][i],
			obb.orientation[1][i],
			obb.orientation[2][i]
		);

		float distance = glm::dot(dir, axis);
		
		// Si la proyeccion del punto (respecto el centro) en cualquiera de los ejes del obb se sale fuera del tamano, entonces no esta contenido, y en caso contrario si lo estara
		if (distance > obb.size[i]) return false;
		if (distance < -obb.size[i]) return false;
	}

	return true;
}
//#include <iostream>
Point ClosestPoint(const Point& point, const OBB& obb)
{
	Point result = obb.position;
	glm::vec3 dir = point - obb.position;

	for (int i = 0; i < 3; ++i)
	{
		// Las columnas de la matriz son los ejes del obb
		glm::vec3 axis(
			obb.orientation[0][i],
			obb.orientation[1][i],
			obb.orientation[2][i]
		);

		// Coordenada de dir en cada uno de los ejes
		float distance = glm::dot(dir, axis);

		// Si se sale fuera del obb en el eje correspondiente, la coordenada del punto mas cercano sera la distancia maxima
		if (distance > obb.size[i])
			distance = obb.size[i];

		if (distance < -obb.size[i]) 
			distance = -obb.size[i];

		// Con esto vamos calculando las coordenadas del resultado en la base definida por orientation y las vamos almacenando en result
		result += axis * distance;
	}

	return result;
}

// Point and Plane
bool PointOnPlane(const Point& point, const Plane& plane)
{
	return CMP(glm::dot(point, plane.normal), plane.distance);
}

Point ClosestPoint(const Point& point, const Plane& plane)
{
	float dot = glm::dot(point, plane.normal);

	return point - plane.normal * (dot - plane.distance);
}

// Point and Line
bool PointOnLine(const Point& point, const Line& line)
{
	Point closest = ClosestPoint(point, line);

	return CMP(glm::distance2(point, closest), 0.f);
}

Point ClosestPoint(const Point& point, const Line& line)
{
	glm::vec3 lineVec = line.end - line.start;

	// Definimos coordenada que recorre la recta, siendo t=0 para line.start y t=1 para line.end
	float t = glm::dot(point - line.start, lineVec) / glm::length2(lineVec);

	// La linea no es infinita, el parametro definido debe estar entre 0 y 1
	t = fmaxf(t, 0.f);
	t = fminf(t, 1.f);

	return line.start + t * lineVec;
}

// Point and Ray
bool PointOnRay(const Point& point, const Ray& ray)
{
	if (point == ray.origin) return true;

	glm::vec3 norm = glm::normalize(point - ray.origin);

	float diff = glm::dot(norm, ray.direction); // asume que el rayo ya esta normalizado

	return CMP(diff, 1.f);
}

Point ClosestPoint(const Point& point, const Ray& ray)
{
	float t = glm::dot(point - ray.origin, ray.direction);

	t = fmaxf(t, 0.f);

	return ray.origin + ray.direction * t;
}

// SHAPE INTERSECTIONS

bool SphereSphere(const Sphere& s1, const Sphere& s2)
{
	float sumRadius = s1.radius + s2.radius;
	return sumRadius * sumRadius > glm::length2(s1.position - s2.position);
}

bool SphereAABB(const Sphere& sphere, const AABB& aabb)
{
	Point closestPoint = ClosestPoint(sphere.position, aabb);

	return glm::distance2(sphere.position, closestPoint) < sphere.radius * sphere.radius;
}

bool SphereOBB(const Sphere& sphere, const OBB& obb)
{
	Point closestPoint = ClosestPoint(sphere.position, obb);

	return glm::distance2(sphere.position, closestPoint) < sphere.radius* sphere.radius;
}

bool SpherePlane(const Sphere& sphere, const Plane& plane)
{
	float distance = fabs(glm::dot(sphere.position, plane.normal) - plane.distance);

	return distance < sphere.radius;
} 

bool AABBAABB(const AABB& aabb1, const AABB& aabb2)
{
	Point min1 = GetMin(aabb1);
	Point max1 = GetMax(aabb1);

	Point min2 = GetMin(aabb2);
	Point max2 = GetMax(aabb2);

	return (min1.x <= max2.x && max1.x >= min2.x) && // comparten coordenada x
		   (min1.y <= max2.y && max1.y >= min2.y) && // comparten coordenada y
		   (min1.z <= max2.z && max1.z >= min2.z);   // comparten coordenada z
}


struct Interval
{
	float min, max;
};

Interval GetInterval(const AABB& aabb, const glm::vec3& axis)
{
	glm::vec3 i = GetMin(aabb);
	glm::vec3 a = GetMax(aabb);

	glm::vec3 vertices[8] =
	{
		glm::vec3(i.x, a.y, a.z),
		glm::vec3(i.x, a.y, i.z),
		glm::vec3(i.x, i.y, a.z),
		glm::vec3(i.x, i.y, i.z),
		glm::vec3(a.x, a.y, a.z),
		glm::vec3(a.x, a.y, i.z),
		glm::vec3(a.x, i.y, a.z),
		glm::vec3(a.x, i.y, i.z),
	};

	Interval result;
	result.min = result.max = glm::dot(axis, vertices[0]);

	for (auto& vertex : vertices)
	{
		float projection = glm::dot(axis, vertex);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}

Interval GetInterval(const OBB& obb, const glm::vec3& axis)
{
	glm::vec3 vertices[8];

	glm::vec3 center = obb.position;
	glm::vec3 extents = obb.size;

	const float* orientation = &obb.orientation[0][0];

	glm::vec3 axises[] =
	{
		glm::vec3(orientation[0], orientation[3], orientation[6]),
		glm::vec3(orientation[1], orientation[4], orientation[7]),
		glm::vec3(orientation[2], orientation[5], orientation[8])
	};

	vertices[0] = center + axises[0]*extents[0] + axises[1]*extents[1] + axises[2]*extents[2];
	vertices[1] = center - axises[0]*extents[0] + axises[1]*extents[1] + axises[2]*extents[2];
	vertices[2] = center + axises[0]*extents[0] - axises[1]*extents[1] + axises[2]*extents[2];
	vertices[3] = center + axises[0]*extents[0] + axises[1]*extents[1] - axises[2]*extents[2];
	vertices[4] = center - axises[0]*extents[0] - axises[1]*extents[1] - axises[2]*extents[2];
	vertices[5] = center + axises[0]*extents[0] - axises[1]*extents[1] - axises[2]*extents[2];
	vertices[6] = center - axises[0]*extents[0] + axises[1]*extents[1] - axises[2]*extents[2];
	vertices[7] = center - axises[0]*extents[0] - axises[1]*extents[1] + axises[2]*extents[2];

	Interval result;
	result.min = result.max = glm::dot(axis, vertices[0]);

	for (int i = 1; i < 8; ++i)
	{
		float projection = glm::dot(axis, vertices[i]);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}

bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(obb, axis);
	//std::cout << std::endl << a.min << " " << a.max << " " << b.min << " " << b.max;
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool AABBOBB(const AABB& aabb, const OBB& obb)
{
	const float* orientation = &obb.orientation[0][0];

	glm::vec3 test[15] =
	{
		glm::vec3(1.f, 0.f, 0.f),									// AABB axis 1
		glm::vec3(0.f, 1.f, 0.f),									// AABB axis 2
		glm::vec3(0.f, 0.f, 1.f),									// AABB axis 3

		glm::vec3(orientation[0], orientation[3], orientation[6]),	// OBB  axis 1
		glm::vec3(orientation[1], orientation[4], orientation[7]),	// OBB  axis 2
		glm::vec3(orientation[2], orientation[5], orientation[8])	// OBB  axis 3
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = glm::cross(test[i+3], test[0]); // EN SZAUER FALTA EL +3, ES UNA ERRATA.
		test[6 + i * 3 + 1] = glm::cross(test[i+3], test[1]);
		test[6 + i * 3 + 2] = glm::cross(test[i+3], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		/*if (glm::length2(test[i]) < 1e-6) // Ignora ejes cercanos a 0
			continue;*/

		if (!OverlapOnAxis(aabb, obb, test[i])) // Se encuentra un eje en el que estan separados
			return false;
	}

	return true; // todos los ejes están superpuestos
}

bool AABBPlane(const AABB& aabb, const Plane& plane)
{
	float pLen = aabb.size.x * fabsf(plane.normal.x) +
				 aabb.size.y * fabsf(plane.normal.y) +
				 aabb.size.z * fabsf(plane.normal.z);

	float dot = glm::dot(plane.normal, aabb.position);
	float dist = dot - plane.distance;

	return fabsf(dist) <= pLen;
}

bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis)
{
	Interval a = GetInterval(obb1, axis);
	Interval b = GetInterval(obb1, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool OBBOBB(const OBB& obb1, const OBB& obb2)
{
	const float* orientation1 = &obb1.orientation[0][0];
	const float* orientation2 = &obb2.orientation[0][0];

	glm::vec3 test[15] =
	{
		glm::vec3(orientation1[0], orientation1[3], orientation1[6]),	// OBB1 axis 1
		glm::vec3(orientation1[1], orientation1[4], orientation1[7]),	// OBB1 axis 2
		glm::vec3(orientation1[2], orientation1[5], orientation1[8]),	// OBB1 axis 3

		glm::vec3(orientation2[0], orientation2[3], orientation2[6]),	// OBB2 axis 1
		glm::vec3(orientation2[1], orientation2[4], orientation2[7]),	// OBB2 axis 2
		glm::vec3(orientation2[2], orientation2[5], orientation2[8])	// OBB2 axis 3
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = glm::cross(test[i+3], test[0]); // EN SZAUER FALTA EL +3, ES UNA ERRATA.
		test[6 + i * 3 + 1] = glm::cross(test[i+3], test[1]);
		test[6 + i * 3 + 2] = glm::cross(test[i+3], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(obb1, obb2, test[i])) // Se encuentra un eje en el que estan separados
			return false;
	}

	return true; // todos los ejes están superpuestos
}

bool OBBPlane(const OBB& obb, const Plane& plane)
{
	const float* orientation = &obb.orientation[0][0];

	glm::vec3 rot[] =
	{
		glm::vec3(orientation[0], orientation[3], orientation[6]),
		glm::vec3(orientation[1], orientation[4], orientation[7]),
		glm::vec3(orientation[2], orientation[5], orientation[8])
	};

	glm::vec3 normal = plane.normal;

	float pLen = obb.size.x * fabsf(glm::dot(normal, rot[0])) +
				 obb.size.y * fabsf(glm::dot(normal, rot[1])) +
				 obb.size.z * fabsf(glm::dot(normal, rot[2]));

	float dot = glm::dot(plane.normal, obb.position);
	float dist = dot - plane.distance;

	return fabsf(dist) <= pLen;
}

bool PlanePlane(const Plane& plane1, const Plane& plane2)
{
	//glm::vec3 cross = glm::cross(plane1.normal, plane2.normal);
	//return CMP(glm::length2(cross), 0.f);
	return CMP(fabsf(glm::dot(plane1.normal, plane2.normal)), 1.f);
}


float Raycast(const Sphere& sphere, const Ray& ray)
{
	glm::vec3 e = sphere.position - ray.origin;

	float rSq = sphere.radius * sphere.radius;
	float eSq = glm::length2(e);

	float a = glm::dot(e, ray.direction);

	float bSq = eSq - a * a;

	float f = sqrt(rSq - bSq);

	if (rSq - (eSq - a * a) < 0.f)
		return -1.f; // Invalido

	if (eSq < rSq)
		return a + f; // Direccion inversa

	return a - f; // Interseccion normal
}

bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	glm::vec3 e = sphere.position - ray.origin;

	float rSq = sphere.radius * sphere.radius;
	float eSq = glm::length2(e);

	float a = glm::dot(e, ray.direction);

	float bSq = eSq - a * a;

	float f = sqrt(rSq - bSq);

	if (rSq - (eSq - a * a) < 0.f)
		return false; // Invalido

	float t = a - f; // Interseccion normal

	if (eSq < rSq)
		t = a + f; // Direccion inversa

	if (outResult == 0) return true;

	outResult->t = t;
	outResult->hit = true;
	outResult->point = ray.origin + ray.direction * t;
	outResult->normal = glm::normalize(outResult->point - sphere.position);

	return true;
}

float Raycast(const AABB& aabb, const Ray& ray)
{
	glm::vec3 min = GetMin(aabb);
	glm::vec3 max = GetMax(aabb);

	// Calculamos las distancias a las intersecciones con los 6 planos que conforman el AABB
	float rayComponent;

	rayComponent = CMP(ray.direction.x, 0.f) ? 1E-6f : ray.direction.x;
	float t1 = (min.x - ray.origin.x) / rayComponent;
	float t2 = (max.x - ray.origin.x) / rayComponent;
	
	rayComponent = CMP(ray.direction.y, 0.f) ? 1E-6f : ray.direction.y;
	float t3 = (min.y - ray.origin.y) / rayComponent;
	float t4 = (max.y - ray.origin.y) / rayComponent;

	rayComponent = CMP(ray.direction.z, 0.f) ? 1E-6f : ray.direction.z;
	float t5 = (min.z - ray.origin.z) / rayComponent;
	float t6 = (max.z - ray.origin.z) / rayComponent;

	// El rayo terminara en uno de los tres pares de planos (t1 t2), (t3, t4) y (t5, t6). De acabar en una de esas parejas, sera en la mas cercana, 
	// pero de estas tres parejas debera ser la mas alejada porque sino estaria chocando con un plano imaginario y non el AABB 
	float tmin = fmaxf(fmaxf(
		fminf(t1, t2),
		fminf(t3, t4)),
		fminf(t5, t6)
	);

	// Si el origen esta dentro del AABB entonces habra que buscar la distancia maxima dentro de las parejas porque el minimo sera negativo y esa solucion no es deseada.
	float tmax = fminf(fminf(
		fmaxf(t1, t2),
		fmaxf(t3, t4)),
		fmaxf(t5, t6)
	);

	// Ahora tmin es el primer punto con el que intersecta y tmax es el segundo punto con el que intersecta

	if (tmax < 0.f) return -1.f;	// El rayo no colisiona con ningun plano
	if (tmin > tmax) return -1.f;	// El rayo colisiona con algun plano pero no llega a colisionar con el AABB
	if (tmin < 0.f) return tmax;	// Esta dentro del AABB
	return tmin;					// Esta fuera del AABB y colisiona
}

bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	glm::vec3 min = GetMin(aabb);
	glm::vec3 max = GetMax(aabb);

	// Calculamos las distancias a las intersecciones con los 6 planos que conforman el AABB
	float rayComponent;

	union {
		float t1, t2, t3, t4, t5, t6;
		float ts[6];
	};

	rayComponent = CMP(ray.direction.x, 0.f) ? 1E-6f : ray.direction.x;
	t1 = (min.x - ray.origin.x) / rayComponent;
	t2 = (max.x - ray.origin.x) / rayComponent;

	rayComponent = CMP(ray.direction.y, 0.f) ? 1E-6f : ray.direction.y;
	t3 = (min.y - ray.origin.y) / rayComponent;
	t4 = (max.y - ray.origin.y) / rayComponent;

	rayComponent = CMP(ray.direction.z, 0.f) ? 1E-6f : ray.direction.z;
	t5 = (min.z - ray.origin.z) / rayComponent;
	t6 = (max.z - ray.origin.z) / rayComponent;

	// El rayo terminara en uno de los tres pares de planos (t1 t2), (t3, t4) y (t5, t6). De acabar en una de esas parejas, sera en la mas cercana, 
	// pero de estas tres parejas debera ser la mas alejada porque sino estaria chocando con un plano imaginario y non el AABB 
	float tmin = fmaxf(fmaxf(
		fminf(t1, t2),
		fminf(t3, t4)),
		fminf(t5, t6)
	);

	// Si el origen esta dentro del AABB entonces habra que buscar la distancia maxima dentro de las parejas porque el minimo sera negativo y esa solucion no es deseada.
	float tmax = fminf(fminf(
		fmaxf(t1, t2),
		fmaxf(t3, t4)),
		fmaxf(t5, t6)
	);

	// Ahora tmin es el primer punto con el que intersecta y tmax es el segundo punto con el que intersecta

	if (tmax < 0.f) return false;	// El rayo no colisiona con ningun plano
	if (tmin > tmax) return false;	// El rayo colisiona con algun plano pero no llega a colisionar con el AABB

	if (outResult == 0) return true;

	float t = tmin;				// Esta fuera del AABB y colisiona
	if (tmin < 0.f) t = tmax;	// Esta dentro del AABB

	outResult->t = t;
	outResult->hit = true;
	outResult->point = ray.origin + ray.direction * t;

	glm::vec3 normals[] = {
		{-1.f,  0.f,  0.f},
		{ 0.f, -1.f,  0.f},
		{ 0.f,  0.f, -1.f},
		{ 1.f,  0.f,  0.f},
		{ 0.f,  1.f,  0.f},
		{ 0.f,  0.f,  1.f},
	};

	for (int i = 0; i < 6; ++i)
	{
		if (CMP(t, ts[i]))
			outResult->normal = normals[i];
	}

	return true;
}

float Raycast(const OBB& obb, const Ray& ray)
{
	const float* orientation = &obb.orientation[0][0];

	glm::vec3 X(orientation[0], orientation[3], orientation[6]);
	glm::vec3 Y(orientation[1], orientation[4], orientation[7]);
	glm::vec3 Z(orientation[2], orientation[5], orientation[8]);

	glm::vec3 p = obb.position - ray.origin;
	
	glm::vec3 f(
		glm::dot(X, ray.direction),
		glm::dot(Y, ray.direction),
		glm::dot(Z, ray.direction)
	);

	glm::vec3 e(
		glm::dot(X, p),
		glm::dot(Y, p),
		glm::dot(Z, p)
	);

	float t[6] = { 0,0,0,0,0,0 };

	for (int i = 0; i < 3; ++i)
	{
		if (CMP(f[i], 0.f))
		{
			if (-e[i] - obb.size[i] > 0 || -e[i] + obb.size[i] < 0)
				return -1;

			f[i] = 1E-6f;
		}
		t[i * 2 + 0] = (e[i] + obb.size[i]) / f[i]; // min
		t[i * 2 + 1] = (e[i] - obb.size[i]) / f[i]; // max
	}

	float tmin = fmaxf(fmaxf(
		fminf(t[0], t[1]),
		fminf(t[2], t[3])),
		fminf(t[4], t[5])
	);

	float tmax = fminf(fminf(
		fmaxf(t[0], t[1]),
		fmaxf(t[2], t[3])),
		fmaxf(t[4], t[5])
	);

	if (tmax < 0.f) return -1.f;	// El rayo no colisiona con ningun plano
	if (tmin > tmax) return -1.f;	// El rayo colisiona con algun plano pero no llega a colisionar con el AABB
	if (tmin < 0.f) return tmax;	// Esta dentro del AABB
	return tmin;					// Esta fuera del AABB y colisiona
}

bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	const float* orientation = &obb.orientation[0][0];

	glm::vec3 X(orientation[0], orientation[3], orientation[6]);
	glm::vec3 Y(orientation[1], orientation[4], orientation[7]);
	glm::vec3 Z(orientation[2], orientation[5], orientation[8]);

	glm::vec3 p = obb.position - ray.origin;

	glm::vec3 f(
		glm::dot(X, ray.direction),
		glm::dot(Y, ray.direction),
		glm::dot(Z, ray.direction)
	);

	glm::vec3 e(
		glm::dot(X, p),
		glm::dot(Y, p),
		glm::dot(Z, p)
	);

	float t[6] = { 0,0,0,0,0,0 };

	for (int i = 0; i < 3; ++i)
	{
		if (CMP(f[i], 0.f))
		{
			if (-e[i] - obb.size[i] > 0 || -e[i] + obb.size[i] < 0)
				return -1;

			f[i] = 1E-6f;
		}
		t[i * 2 + 0] = (e[i] + obb.size[i]) / f[i]; // min
		t[i * 2 + 1] = (e[i] - obb.size[i]) / f[i]; // max
	}

	float tmin = fmaxf(fmaxf(
		fminf(t[0], t[1]),
		fminf(t[2], t[3])),
		fminf(t[4], t[5])
	);

	float tmax = fminf(fminf(
		fmaxf(t[0], t[1]),
		fmaxf(t[2], t[3])),
		fmaxf(t[4], t[5])
	);

	if (tmax < 0.f) return false;	// El rayo no colisiona con ningun plano
	if (tmin > tmax) return false;	// El rayo colisiona con algun plano pero no llega a colisionar con el AABB

	if (outResult == 0) return true;
	
	float tResult = tmin;			// Esta fuera del AABB y colisiona
	if (tmin < 0.f) tResult = tmax;	// Esta dentro del AABB

	outResult->t = tResult;
	outResult->hit = true;
	outResult->point = ray.origin + ray.direction * tResult;

	glm::vec3 normals[] = {
		X, Y, Z,
		-X, -Y, -Z
	};

	for (int i = 0; i < 6; ++i)
	{
		if (CMP(tResult, t[i]))
			outResult->normal = normals[i];
	}

	return true;
}

float Raycast(const Plane& plane, const Ray& ray)
{
	float nd = glm::dot(ray.direction, plane.normal);
	float pn = glm::dot(ray.origin, plane.normal);

	if (nd >= 0.f) return -1;	// El rayo mira en una direccion opuesta al plano (misma direccion que la normal)

	float t = (plane.distance - pn) / nd;

	if (t >= 0.f) return t;		// Colisiona en una distancia positiva

	return -1.f;				// Colisiona en una distancia negativa (no cuenta)
}

bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	float nd = glm::dot(ray.direction, plane.normal);
	float pn = glm::dot(ray.origin, plane.normal);

	if (nd >= 0.f) return false;	// El rayo mira en una direccion opuesta al plano (misma direccion que la normal)

	float t = (plane.distance - pn) / nd;

	if (t >= 0.f)
	{
		if (outResult == 0) return true;

		outResult->t = t;
		outResult->hit = true;
		outResult->point = ray.origin + ray.direction * t;
		outResult->normal = glm::normalize(plane.normal);

		return true;			// Colisiona en una distancia positiva
	}

	return false;					// Colisiona en una distancia negativa (no cuenta)
}

bool Linetest(const Sphere& sphere, const Line& line)
{
	Point closest = ClosestPoint(sphere.position, line);
	float distSq = glm::distance2(sphere.position, closest);
	return distSq <= sphere.radius * sphere.radius;
}

bool Linetest(const AABB& aabb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = glm::normalize(line.end - line.start);
	float t = Raycast(aabb, ray);

	return t >= 0.f && t * t <= LengthSq(line);
}

bool Linetest(const OBB& obb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = glm::normalize(line.end - line.start);
	float t = Raycast(obb, ray);

	return t >= 0.f && t * t <= LengthSq(line);
}

bool Linetest(const Plane& plane, const Line& line)
{
	glm::vec3 ab = line.end - line.start;

	float nA = glm::dot(plane.normal, line.start);
	float nAB = glm::dot(plane.normal, ab);

	if (CMP(nAB, 0.f)) return false;

	float t = (plane.distance - nA) / nAB;

	return t >= 0.f && t <= 1.f;
}

// TRIANGLES

bool PointInTriangle(const Point& point, const Triangle& triangle)
{
	glm::vec3 a = triangle.a - point;
	glm::vec3 b = triangle.b - point;
	glm::vec3 c = triangle.c - point;

	glm::vec3 normPBC = glm::cross(b, c);
	glm::vec3 normPCA = glm::cross(c, a);
	glm::vec3 normPAB = glm::cross(a, b);

	if (glm::dot(normPBC, normPCA) < 0.f) return false;
	if (glm::dot(normPBC, normPAB) < 0.f) return false;

	return true;
}

Plane FromTriangle(const Triangle& triangle)
{
	Plane result;

	result.normal = glm::normalize(
		glm::cross(triangle.b - triangle.a, triangle.c - triangle.a)
	);

	result.distance = glm::dot(result.normal, triangle.a);

	return result;
}

Point ClosestPoint(const Point& point, const Triangle& triangle)
{
	Plane plane = FromTriangle(triangle);

	// Se comprueba si el punto se encuentra sobre el triangulo. Si no el punto mas cercano estara en uno de los bordes del triangulo
	Point closest = ClosestPoint(point, plane);
	if (PointInTriangle(closest, triangle)) return closest;

	// Calcula el punto mas cercano en cada uno de los bordes del triangulo
	Point c1 = ClosestPoint(point, Line(triangle.a, triangle.b));
	Point c2 = ClosestPoint(point, Line(triangle.b, triangle.c));
	Point c3 = ClosestPoint(point, Line(triangle.c, triangle.a));

	// Calcula las distancias del punto a los puntos mas cercanos de los bordes del triangulo
	float distSq1 = glm::distance2(point, c1);
	float distSq2 = glm::distance2(point, c2);
	float distSq3 = glm::distance2(point, c3);

	// Devuelve el punto cuya distancia al punto sesa mas pequena
	if (distSq1 < distSq2 && distSq1 < distSq3) return c1;
	if (distSq2 < distSq1 && distSq2 < distSq3) return c2;
	return c3;
}

bool TriangleSphere(const Triangle& triangle, const Sphere& sphere)
{
	Point closest = ClosestPoint(sphere.position, triangle);
	float distSq = glm::distance2(closest, sphere.position);
	return distSq <= sphere.radius * sphere.radius;
}

Interval GetInterval(const Triangle& triangle, const glm::vec3& axis)
{
	Interval result;

	result.min = glm::dot(axis, triangle.points[0]);
	result.max = result.min;

	for (int i = 1; i < 3; ++i)
	{
		float value = glm::dot(axis, triangle.points[i]);

		result.min = fminf(result.min, value);
		result.max = fmaxf(result.max, value);
	}

	return result;
}

bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const glm::vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(triangle, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool TriangleAABB(const Triangle& triangle, const AABB& aabb)
{
	glm::vec3 edge0 = triangle.b - triangle.a;
	glm::vec3 edge1 = triangle.c - triangle.b;
	glm::vec3 edge2 = triangle.a - triangle.c;

	glm::vec3 faceNormal0(1.f, 0.f, 0.f);
	glm::vec3 faceNormal1(0.f, 1.f, 0.f);
	glm::vec3 faceNormal2(0.f, 0.f, 1.f);

	glm::vec3 test[13] = {
		faceNormal0,				// AABB Axis 1
		faceNormal1,				// AABB Axis 2
		faceNormal2,				// AABB Axis 3

		glm::cross(edge0, edge1),	// Triangle Normal

		glm::cross(edge0, faceNormal0), glm::cross(edge0, faceNormal1), glm::cross(edge0, faceNormal2),
		glm::cross(edge1, faceNormal0), glm::cross(edge1, faceNormal1), glm::cross(edge1, faceNormal2),
		glm::cross(edge2, faceNormal0), glm::cross(edge2, faceNormal1), glm::cross(edge2, faceNormal2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(aabb, triangle, test[i]))
			return false;
	}

	return true;
}

bool OverlapOnAxis(const OBB& obb, const Triangle& triangle, const glm::vec3& axis)
{
	Interval a = GetInterval(obb, axis);
	Interval b = GetInterval(triangle, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool TriangleOBB(const Triangle& triangle, const OBB& obb)
{
	glm::vec3 edge0 = triangle.b - triangle.a;
	glm::vec3 edge1 = triangle.c - triangle.b;
	glm::vec3 edge2 = triangle.a - triangle.c;

	const float* orientation = &obb.orientation[0][0];

	glm::vec3 faceNormal0(orientation[0], orientation[3], orientation[6]);
	glm::vec3 faceNormal1(orientation[1], orientation[4], orientation[7]);
	glm::vec3 faceNormal2(orientation[2], orientation[5], orientation[8]);

	glm::vec3 test[13] = {
		faceNormal0,				// OBB Axis 1
		faceNormal1,				// OBB Axis 2
		faceNormal2,				// OBB Axis 3

		glm::cross(edge0, edge1),	// Triangle Normal

		glm::cross(edge0, faceNormal0), glm::cross(edge0, faceNormal1), glm::cross(edge0, faceNormal2),
		glm::cross(edge1, faceNormal0), glm::cross(edge1, faceNormal1), glm::cross(edge1, faceNormal2),
		glm::cross(edge2, faceNormal0), glm::cross(edge2, faceNormal1), glm::cross(edge2, faceNormal2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(obb, triangle, test[i]))
			return false;
	}

	return true;
}


bool TrianglePlane(const Triangle& triangle, const Plane& plane)
{
	float side1 = PlaneEquation(triangle.a, plane);
	float side2 = PlaneEquation(triangle.b, plane);
	float side3 = PlaneEquation(triangle.c, plane);

	if (CMP(side1, 0.f) && CMP(side2, 0.f) && CMP(side3, 0.f)) return true; // Los tres puntos del triangulo estan contenidos en el plano

	if (side1 > 0.f && side2 > 0.f && side3 > 0.f) return false; // Si los tres puntos estan por encima del plano

	if (side1 < 0.f && side2 < 0.f && side3 < 0.f) return false; // Si los tres puntos estan por debajo del plano

	return true; // Intersectan porque hay puntos tanto debajo como arriba del plano
}


bool OverlapOnAxis(const Triangle& triangle1, const Triangle& triangle2, const glm::vec3& axis)
{
	Interval a = GetInterval(triangle1, axis);
	Interval b = GetInterval(triangle2, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool TriangleTriangle(const Triangle& triangle1, const Triangle& triangle2)
{
	glm::vec3 triangle1Edge0 = triangle1.b - triangle1.a;
	glm::vec3 triangle1Edge1 = triangle1.c - triangle1.b;
	glm::vec3 triangle1Edge2 = triangle1.a - triangle1.c;

	glm::vec3 triangle2Edge0 = triangle2.b - triangle2.a;
	glm::vec3 triangle2Edge1 = triangle2.c - triangle2.b;
	glm::vec3 triangle2Edge2 = triangle2.a - triangle2.c;

	glm::vec3 test[] = {
		glm::cross(triangle1Edge0, triangle1Edge1),		// Normal of triangle 1

		glm::cross(triangle1Edge0, triangle1Edge1),		// Normal of triangle 2

		glm::cross(triangle2Edge0, triangle1Edge0), glm::cross(triangle2Edge0, triangle1Edge1), glm::cross(triangle2Edge0, triangle1Edge1),
		glm::cross(triangle2Edge1, triangle1Edge0), glm::cross(triangle2Edge1, triangle1Edge1), glm::cross(triangle2Edge1, triangle1Edge1),
		glm::cross(triangle2Edge2, triangle1Edge0), glm::cross(triangle2Edge2, triangle1Edge1), glm::cross(triangle2Edge2, triangle1Edge1)
	};

	for (int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(triangle1, triangle2, test[i]))
			return false;
	}

	return true;
}

glm::vec3 SatCrossEdge(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
{
	glm::vec3 ab = a - b;
	glm::vec3 cd = c - d;
	glm::vec3 result = glm::cross(ab, cd);

	if (!CMP(glm::length2(result), 0.f))
		return result; // not parallel

	glm::vec3 axis = glm::cross(ab, c - a);

	result = glm::cross(ab, axis);

	if (!CMP(glm::length2(result), 0.f))
		return result; // not parallel

	return glm::vec3();
}

bool TriangleTriangleRobust(const Triangle& triangle1, const Triangle& triangle2)
{
	glm::vec3 test[] = {
		SatCrossEdge(triangle1.a, triangle1.b, triangle1.b, triangle1.c),		// Normal of triangle 1

		SatCrossEdge(triangle2.a, triangle2.b, triangle2.b, triangle2.c),		// Normal of triangle 2

		SatCrossEdge(triangle2.a, triangle2.b, triangle1.a, triangle1.b), 
		SatCrossEdge(triangle2.a, triangle2.b, triangle1.b, triangle1.c),
		SatCrossEdge(triangle2.a, triangle2.b, triangle1.c, triangle1.a),

		SatCrossEdge(triangle2.b, triangle2.c, triangle1.a, triangle1.b), 
		SatCrossEdge(triangle2.b, triangle2.c, triangle1.b, triangle1.c),
		SatCrossEdge(triangle2.b, triangle2.c, triangle1.c, triangle1.a),

		SatCrossEdge(triangle2.c, triangle2.a, triangle1.a, triangle1.b), 
		SatCrossEdge(triangle2.c, triangle2.a, triangle1.b, triangle1.c),
		SatCrossEdge(triangle2.c, triangle2.a, triangle1.c, triangle1.a)
	};

	for (int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(triangle1, triangle2, test[i]))
		{
			if (!CMP(glm::length2(test[i]), 0.f))
				return false;
		}
	}

	return true;
}

glm::vec3 project(const glm::vec3& length, const glm::vec3& direction)
{
	float dot = glm::dot(length, direction);
	float magSq = glm::length2(direction);
	return direction * (dot / magSq);
}

glm::vec3 Barycentric(const Point& point, const Triangle& triangle)
{
	glm::vec3 ap = point - triangle.a;
	glm::vec3 bp = point - triangle.b;
	glm::vec3 cp = point - triangle.c;

	glm::vec3 ab = triangle.b - triangle.a;
	glm::vec3 ac = triangle.c - triangle.a;
	glm::vec3 bc = triangle.c - triangle.b;
	glm::vec3 cb = triangle.b - triangle.c;
	glm::vec3 ca = triangle.a - triangle.c;

	glm::vec3 v = ab - project(ab, cb);
	float a = 1.f - glm::dot(v, ap) / glm::dot(v, ab);

	v = bc - project(bc, ac);
	float b = 1.f - glm::dot(v, bp) / glm::dot(v, bc);

	v = ca - project(ca, ab);
	float c = 1.f - glm::dot(v, cp) / glm::dot(v, ca);

	return glm::vec3(a, b, c);
}

float Raycast(const Triangle& triangle, const Ray& ray)
{
	Plane plane = FromTriangle(triangle);
	float t = Raycast(plane, ray);
	
	if (t < 0.f) return t;

	Point result = ray.origin + ray.direction * t;

	glm::vec3 barycentric = Barycentric(result, triangle);

	if (barycentric.x >= 0.f && barycentric.x <= 1.f &&
		barycentric.y >= 0.f && barycentric.y <= 1.f &&
		barycentric.z >= 0.f && barycentric.z <= 1.f)
		return t;

	return -1.f;
}

bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	Plane plane = FromTriangle(triangle);
	RaycastResult planeResult;

	if (!Raycast(plane, ray, &planeResult)) return false;
	
	glm::vec3 barycentric = Barycentric(planeResult.point, triangle);

	if (barycentric.x >= 0.f && barycentric.x <= 1.f &&
		barycentric.y >= 0.f && barycentric.y <= 1.f &&
		barycentric.z >= 0.f && barycentric.z <= 1.f)
	{
		if (outResult == 0) return true;

		outResult->t = planeResult.t;
		outResult->hit = true;
		outResult->point = planeResult.point;
		outResult->normal = plane.normal;

		return true;
	}

	return false;
}

bool Linetest(const Triangle& triangle, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = glm::normalize(line.end - line.start);

	float t = Raycast(triangle, ray);

	return t >= 0 && t * t <= LengthSq(line);
}
//
//Triangle GetTriangle(const Mesh& mesh, int triangleIndex)
//{
//	Triangle result;
//
//	result.a = mesh.vertices[mesh.indices[3 * triangleIndex + 0]].position;
//	result.b = mesh.vertices[mesh.indices[3 * triangleIndex + 1]].position;
//	result.c = mesh.vertices[mesh.indices[3 * triangleIndex + 2]].position;
//
//	return result;
//}


Point Intersection(const Plane& p1, const Plane& p2, const Plane& p3)
{
	// Resuelve la ecuacion dot(p1.normal, x) = p1.distance, que es la ecuacion del plano, para cada uno de los planos.

	glm::mat3 D = glm::transpose(glm::mat3(p1.normal, p2.normal, p3.normal));

	glm::vec3 A(p1.distance, p2.distance, p3.distance);

	float detD = glm::determinant(D);

	glm::mat3 Dx, Dy, Dz;
	Dx = Dy = Dz = D;

	Dx[0] = A;
	Dy[1] = A;
	Dz[2] = A;

	float detDx = glm::determinant(Dx);
	float detDy = glm::determinant(Dy);
	float detDz = glm::determinant(Dz);

	return Point(
		detDx / detD,
		detDy / detD,
		detDz / detD
	);
}

void GetCorners(const Frustum& f, glm::vec3* outCorners)
{
	outCorners[0] = Intersection(f.near, f.top,    f.left );
	outCorners[1] = Intersection(f.near, f.top,    f.right);
	outCorners[2] = Intersection(f.near, f.bottom, f.left );
	outCorners[3] = Intersection(f.near, f.bottom, f.right);
	outCorners[4] = Intersection(f.far,  f.top,    f.left );
	outCorners[5] = Intersection(f.far,  f.top,    f.right);
	outCorners[6] = Intersection(f.far,  f.bottom, f.left );
	outCorners[7] = Intersection(f.far,  f.bottom, f.right);
}

bool Intersects(const Frustum& f, const Point& p)
{
	for (int i = 0; i < 6; ++i)
	{
		glm::vec3 normal = f.planes[i].normal;
		float dist = f.planes[i].distance;
		float side = glm::dot(p, normal) - dist;

		if (side < 0.f) return false;
	}
	return true;
}

bool Intersects(const Frustum& f, const Sphere& s)
{
	for (int i = 0; i < 6; ++i)
	{
		glm::vec3 normal = f.planes[i].normal;
		float dist = f.planes[i].distance;
		float side = glm::dot(s.position, normal) - dist;

		if (side < -s.radius) return false;
	}

	return true;
}

float Classify(const AABB& aabb, const Plane& plane)
{
	float r = fabsf(aabb.size.x * plane.normal.x)
			+ fabsf(aabb.size.y * plane.normal.y)
			+ fabsf(aabb.size.z * plane.normal.z);

	float d = glm::dot(plane.normal, aabb.position) - plane.distance;

	if (fabsf(d) < r) return 0.f;

	if (d < 0.f) return d + r;

	return d - r;
}

float Classify(const OBB& obb, const Plane& plane)
{
	glm::vec3 normal = MultiplyVector(plane.normal, obb.orientation);

	// Calcula la distancia desde el centro del obb hasta un vertice proyectado en la normal de la superficie
	float r = fabsf(obb.size.x * normal.x)
			+ fabsf(obb.size.y * normal.y)
			+ fabsf(obb.size.z * normal.z);
	// Se hace con valores absolutos porque asi se obtiene directamente la del vertice con mayor contribucion
	
	// Calcula la distancia desde (0,0,0) hasta la posicion del obb proyectada en la normal de la superficie
	float d = glm::dot(plane.normal, obb.position) - plane.distance;

	if (fabsf(d) < r) return 0.f; // entonces estan intersecando

	if (d < 0.f) return d + r; // se encuentra en la direccion contraria al plano

	return d - r; // se encuentra en direccion de la normal del plano
}

bool Intersects(const Frustum& f, const AABB& aabb)
{
	for (int i = 0; i < 6; ++i)
	{
		if (Classify(aabb, f.planes[i]) < 0)
			return false;
	}

	return true;
}

bool Intersects(const Frustum& f, const OBB& obb)
{
	for (int i = 0; i < 6; ++i)
	{
		if (Classify(obb, f.planes[i]) < 0)
			return false;
	}

	return true;
}

glm::vec3 Unproject(
	int xViewport, int yViewport,
	float zViewport,
	int width, int height,
	const glm::mat4& camMatrix
)
{
	yViewport = height - yViewport;

	if (zViewport < -1.f) zViewport = -1.f;
	else if (zViewport > 1.f) zViewport = 1.f;

	glm::vec4 ndcSpace = {
		2.f * xViewport / (float)width - 1.f,
		2.f * yViewport / (float)height - 1.f,
		zViewport,
		1.f
	};
	
	glm::mat4 inverseMatrix = glm::inverse(camMatrix);

	glm::vec4 result = inverseMatrix * ndcSpace;

	if (!CMP(result.w, 0.f)) 
		return glm::vec3(result) / result.w;

	return glm::vec3(result);
}

Ray GetPickRay(
	int xViewport, int yViewport,
	int width, int height,
	const glm::mat4& camMatrix
)
{
	glm::vec3 nearPoint = Unproject(xViewport, yViewport, -1.f, width, height, camMatrix);
	glm::vec3 farPoint = Unproject(xViewport, yViewport, 1.f, width, height, camMatrix);
	
	return Ray(
		nearPoint,
		glm::normalize(farPoint - nearPoint)
	);
}

void ResetRaycastResult(RaycastResult* outResult)
{
	if (outResult == 0) return;
	
	outResult->t = -1.f;
	outResult->hit = false;
	outResult->normal = glm::vec3(0.f, 0.f, 1.f);
	outResult->point = glm::vec3(0.f, 0.f, 0.f);
}