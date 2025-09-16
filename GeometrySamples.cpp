#include "GeometrySamples.h"

std::vector<Mesh*> loadedMeshes;

Mesh* CreatePlaneMesh(Color color)
{
	Mesh* mesh = new Mesh();

	/*static bool firstTime = true;
	if (!firstTime) return mesh;
	firstTime = false;*/

	loadedMeshes.push_back(mesh);

	glm::vec3 up(0.f, 1.f, 0.f);

	std::vector<Vertex> vertices = {
		Vertex(glm::vec3(-0.5f, 0.f, -0.5f), up, color),
		Vertex(glm::vec3(0.5f, 0.f, -0.5f), up, color),
		Vertex(glm::vec3(0.5f, 0.f,  0.5f), up, color),
		Vertex(glm::vec3(-0.5f, 0.f,  0.5f), up, color)
	};

	std::vector<GLuint> indices = {
		2, 1, 0,
		0, 3, 2
	};

	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	mesh->CreateBuffers();
	//mesh->Accelerate();

	return mesh;
}

Mesh* CreateCubeMesh(Color color)
{
	Mesh* mesh = new Mesh();

	/*static bool firstTime = true;
	if (!firstTime) return mesh;
	firstTime = false;*/

	loadedMeshes.push_back(mesh);

	std::vector<Vertex> vertices = {
		// Front face
		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.f,  0.f,  1.f), color),
		Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.f,  0.f,  1.f), color),
		Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.f,  0.f,  1.f), color),
		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.f,  0.f,  1.f), color),
															    
		// Back face										    
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.f,  0.f, -1.f), color),
		Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.f,  0.f, -1.f), color),
		Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.f,  0.f, -1.f), color),
		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.f,  0.f, -1.f), color),
															    
		// Top face											    
		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.f,  1.f,  0.f), color),
		Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.f,  1.f,  0.f), color),
		Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.f,  1.f,  0.f), color),
		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.f,  1.f,  0.f), color),

		// Bottom face
		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.f, -1.f,  0.f), color),
		Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.f, -1.f,  0.f), color),
		Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.f, -1.f,  0.f), color),
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.f, -1.f,  0.f), color),

		// Right face
		Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.f,  0.f,  0.f), color),
		Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.f,  0.f,  0.f), color),
		Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.f,  0.f,  0.f), color),
		Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.f,  0.f,  0.f), color),
															    
		// Left face										    
		Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.f,  0.f,  0.f), color),
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.f,  0.f,  0.f), color),
		Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.f,  0.f,  0.f), color),
		Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.f,  0.f,  0.f), color)
	};

	std::vector<GLuint> indices = {
		// Front face
		0, 1, 2,
		2, 3, 0,

		// Back face
		5, 4, 6,
		6, 4, 7,

		// Top face
		8, 9, 10,
		10, 11, 8,

		// Bottom face
		12, 14, 13,
		12, 15, 14,

		// Right face
		16, 17, 18,
		18, 19, 16,

		// Left face
		20, 22, 21,
		23, 22, 20
	};
	
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	mesh->CreateBuffers();
	mesh->Accelerate();

	return mesh;
}

Mesh* CreateSphereMesh(int nStacks, int nSlices, Color color)
{
	Mesh* mesh = new Mesh();

	loadedMeshes.push_back(mesh);

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// https://danielsieger.com/blog/2021/03/27/generating-spheres.html
	const float PI = 3.1415926535897932384626433832795028841971694f;

	// Add top vertex
	vertices.push_back({
		glm::vec3(0.f, 1.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		color
	});

	// Generate vertices per stack / slice
	for (int i = 0; i < nStacks; i++)
	{
		float phi = PI * (i + 1) / ((float)nStacks);
		for (int j = 0; j < nSlices; j++)
		{
			float theta = 2.f * PI * j / ((float)nSlices);
			glm::vec3 position(
				std::sin(phi) * std::cos(theta),
				std::cos(phi),
				std::sin(phi) * std::sin(theta)
			);

			vertices.push_back({ position, position, color });
		}
	}

	// Add bottom vertex
	vertices.push_back({
		glm::vec3(0.f, -1.f, 0.f),
		glm::vec3(0.f, -1.f, 0.f),
		color
	});

	// Add top / bottom triangles
	for (int i = 0; i < nSlices; ++i)
	{
		indices.push_back(0);
		indices.push_back((i + 1) % nSlices + 1);
		indices.push_back(i + 1);

		indices.push_back(i + nSlices * (nStacks - 2) + 1);
		indices.push_back((i + 1) % nSlices + nSlices * (nStacks - 2) + 1);
		indices.push_back(vertices.size() - 1);
	}

	// Add quads per stack / slice
	for (int j = 0; j < nStacks - 2; j++)
	{
		int j0 = j * nSlices + 1;
		int j1 = (j + 1) * nSlices + 1;

		for (int i = 0; i < nSlices; i++)
		{
			int i0 = j0 + i;
			int i1 = j0 + (i + 1) % nSlices;
			int i2 = j1 + (i + 1) % nSlices;
			int i3 = j1 + i;

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}

	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	mesh->CreateBuffers();
	//mesh->Accelerate();

	return mesh;
}

Mesh* CreateCylinderMesh(int sectorCount, Color color)
{
	Mesh* mesh = new Mesh();

	loadedMeshes.push_back(mesh);

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// Top center
	vertices.push_back({
		glm::vec3(0.f, 0.5f, 0.f),
		glm::vec3(0.f, 1.f, 0.f),
		color
	});

	// Bottom center
	vertices.push_back({
		glm::vec3(0.f, -0.5f, 0.f),
		glm::vec3(0.f, -1.f, 0.f),
		color
	});

	float sectorStep = 2.f * glm::pi<float>() / sectorCount;

	// Top and bottom circles
	int firstIndex = vertices.size();
	for (int i = 0; i < sectorCount; i++)
	{
		float x = 0.5f * std::cos(i * sectorStep);
		float z = 0.5f * std::sin(i * sectorStep);

		vertices.push_back({
			glm::vec3(x, 0.5f, z),
			glm::vec3(0.f, 1.f, 0.f),
			color
		});

		vertices.push_back({
			glm::vec3(x, -0.5f, z),
			glm::vec3(0.f, -1.f, 0.f),
			color
		});

		indices.push_back(0);
		indices.push_back(firstIndex + 2 * ((i + 1) % sectorCount));
		indices.push_back(firstIndex + 2 * i);

		indices.push_back(1);
		indices.push_back(firstIndex + 2 * i + 1);
		indices.push_back(firstIndex + 2 * ((i + 1) % sectorCount) + 1);
	}

	// Side
	firstIndex = vertices.size();

	for (int i = 0; i < sectorCount; i++)
	{
		float x = 0.5f * std::cos(i * sectorStep);
		float z = 0.5f * std::sin(i * sectorStep);

		glm::vec3 normal = glm::normalize(glm::vec3(x, 0.f, z));

		vertices.push_back({
			glm::vec3(x, 0.5f, z),
			normal,
			color
			});

		vertices.push_back({
			glm::vec3(x, -0.5f, z),
			normal,
			color
			});

		indices.push_back(firstIndex + 2 * i);
		indices.push_back(firstIndex + 2 * ((i + 1) % sectorCount) + 1);
		indices.push_back(firstIndex + 2 * i + 1);

		indices.push_back(firstIndex + 2 * i);
		indices.push_back(firstIndex + 2 * ((i + 1) % sectorCount));
		indices.push_back(firstIndex + 2 * ((i + 1) % sectorCount) + 1);
	}

	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	mesh->CreateBuffers();
	mesh->Accelerate();

	return mesh;
}

Mesh* CreateConeMesh(int sectorCount, Color color)
{
	Mesh* mesh = new Mesh();

	loadedMeshes.push_back(mesh);

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// Bottom circle
	vertices.push_back({
		glm::vec3(0.f, -0.5f, 0.f),
		glm::vec3(0.f, -1.f, 0.f),
		color
	});

	const float sectorStep = 2.f * glm::pi<float>() / sectorCount;

	int firstIndex = vertices.size();
	
	for (int i = 0; i < sectorCount; ++i)
	{
		float x = 0.5f * std::cos(i * sectorStep);
		float z = 0.5f * std::sin(i * sectorStep);

		vertices.push_back({
			glm::vec3(x, -0.5f, z),
			glm::vec3(0.f, -1.f, 0.f),
			color
			});

		indices.push_back(0);
		indices.push_back(firstIndex + i);
		indices.push_back(firstIndex + (i + 1) % sectorCount);
	}

	// Side
	firstIndex = vertices.size();

	for (int i = 0; i < sectorCount; i++)
	{
		float x = 0.5f * std::cos(i * sectorStep);
		float z = 0.5f * std::sin(i * sectorStep);

		glm::vec3 normal = glm::normalize(glm::vec3(x, 0.5f, z));

		vertices.push_back({
			glm::vec3(x, -0.5f, z),
			normal,
			color
		});

		vertices.push_back({
			glm::vec3(0.f, 0.5f, 0.f),
			glm::normalize(glm::vec3(
				std::cos((i + 0.5f) * sectorStep),
				1.f,
				std::sin((i + 0.5f) * sectorStep)
			)),
			color
		});

		indices.push_back(firstIndex + 2*i);
		indices.push_back(firstIndex + 2*i + 1);
		indices.push_back(firstIndex + 2 * ((i + 1) % sectorCount));
	}

	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	mesh->CreateBuffers();
	mesh->Accelerate();

	return mesh;
}

Mesh* CreateClothMesh(int width, int height, Color color)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	for (int y = 0; y < height; ++y)
	for (int x = 0; x < width; ++x)
	{
		vertices.push_back(Vertex(
			glm::vec3((float)x, (float)y, 0.f),
			glm::vec3(0.f, 0.f, 1.f),
			color
		));
	}

	int numVerticesFace1 = vertices.size();
	
	for (int y = 0; y < height; ++y)
	for (int x = 0; x < width; ++x)
	{
		vertices.push_back(Vertex(
			glm::vec3((float)x, (float)y, 0.f),
			glm::vec3(0.f, 0.f, -1.f),
			color
		));
	}

	for (int y = 0; y < height-1; ++y)
	for (int x = 0; x < width-1; ++x)
	{
		indices.push_back(x + y * width);
		indices.push_back(x + (y + 1) * width);
		indices.push_back((x + 1) + y * width);

		indices.push_back((x + 1) + (y + 1) * width);
		indices.push_back((x + 1) + y * width);
		indices.push_back(x + (y + 1) * width);

		indices.push_back(numVerticesFace1 + x + y * width);
		indices.push_back(numVerticesFace1 + (x + 1) + y * width);
		indices.push_back(numVerticesFace1 + x + (y + 1) * width);

		indices.push_back(numVerticesFace1 + (x + 1) + y * width);
		indices.push_back(numVerticesFace1 + (x + 1) + (y + 1) * width);
		indices.push_back(numVerticesFace1 + x + (y + 1) * width);
	}

	Mesh* mesh = new Mesh(vertices, indices, GL_DYNAMIC_DRAW, false);

	loadedMeshes.push_back(mesh);

	return mesh;
}

void DeleteGeometrySamples()
{
	for (Mesh* mesh : loadedMeshes)
		delete mesh;
}


void DrawOBB(const OBB& obb, Shader& shader, const char* uniformLocation, Color color)
{
	static Mesh* cubicMesh = CreateCubeMesh(color);
	cubicMesh->SetColor(color);

	static Model obbModel(cubicMesh);

	obbModel.scale = 2.f * obb.size;
	obbModel.position = obb.position;
	obbModel.orientation = glm::quat_cast(obb.orientation);

	obbModel.Render(shader, uniformLocation);
}

void DrawAABB(const AABB& aabb, Shader& shader, const char* uniformLocation, Color color)
{
	static Mesh* cubicMesh = CreateCubeMesh(color);
	cubicMesh->SetColor(color);
	static Model aabbModel(cubicMesh);

	aabbModel.scale = 2.f * aabb.size;
	aabbModel.position = aabb.position;

	aabbModel.Render(shader, uniformLocation);
}

void DrawSphere(float radius, const glm::vec3& position, Shader& shader, const char* uniformLocation, Color color)
{
	static Mesh* sphericalMesh = CreateSphereMesh(10, 10, color);
	sphericalMesh->SetColor(color);

	static Model sphereModel(sphericalMesh);

	sphereModel.scale = glm::vec3(radius);
	sphereModel.position = position;

	sphereModel.Render(shader, uniformLocation);
}

void DrawPoint(const glm::vec3& point, Shader& shader, const char* uniformLocation, Color color)
{
	static Mesh* sphericalMesh = CreateSphereMesh(10, 10, color);
	sphericalMesh->SetColor(color);

	static Model pointModel(sphericalMesh);

	const float radius = 0.03f;

	pointModel.scale = glm::vec3(radius);

	pointModel.position = point;

	pointModel.Render(shader, uniformLocation);
}

void DrawLine(const glm::vec3& start, const glm::vec3& end, float radius, Shader& shader, const char* uniformLocation, Color color)
{
	static Mesh* lineMesh = CreateCylinderMesh(10, color);
	lineMesh->SetColor(color);

	static Model lineModel(lineMesh);

	//const float radius = 0.01f;

	lineModel.scale = glm::vec3(radius, glm::distance(start, end), radius);
	lineModel.orientation = glm::quat(glm::vec3(0.f, 1.f, 0.f), glm::normalize(end - start));
	lineModel.position = 0.5f * (start + end);

	lineModel.Render(shader, uniformLocation);
}

void DrawVector(const glm::vec3& start, const glm::vec3& end, Shader& shader, const char* uniformLocation, Color color)
{
	if (start == end) return;
	DrawLine(start, end, 0.01f, shader, uniformLocation, color);
	
	static Mesh* coneMesh = CreateConeMesh(10, color);
	coneMesh->SetColor(color);

	static Model coneModel(coneMesh);

	const float radius = 0.03f;

	coneModel.scale = glm::vec3(radius, 0.1f, radius);
	coneModel.orientation = glm::quat(glm::vec3(0.f, 1.f, 0.f), glm::normalize(end - start));
	coneModel.position = end;

	coneModel.Render(shader, uniformLocation);
}

void DrawRay(const Ray& ray, Shader& shader, const char* uniformLocation, Color color)
{
	if (ray.direction == glm::vec3(0.f)) return;
	//DrawPoint(ray.origin, shader, uniformLocation);
	DrawLine(ray.origin, ray.origin + ray.direction * 100.f, 0.01f, shader, uniformLocation, color);
}

void DrawPlane(const Plane& plane, Shader& shader, const char* uniformLocation, Color color)
{
	static Mesh* planeMesh = CreatePlaneMesh(color);
	planeMesh->SetColor(color);

	static Model planeModel(planeMesh);

	const float size = 10000.f;

	planeModel.scale = glm::vec3(size, 1.f, size);

	planeModel.position = plane.normal * plane.distance;

	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);

	planeModel.orientation = glm::quat(glm::vec3(0.f, 1.f, 0.f), plane.normal);

	planeModel.Render(shader, uniformLocation);
}