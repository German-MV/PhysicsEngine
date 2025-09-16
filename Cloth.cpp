#include "Cloth.h"

Cloth::Cloth() : PhysicsObject(CLOTH) {}

Cloth::Cloth(int width, int height, float spacing, float mass) : width(width), height(height), spacing(spacing), PhysicsObject(CLOTH)
{
	float particleMass = mass / (float)(width * height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			particles.emplace_back(particleMass);
			particles.back().SetPosition(glm::vec3(x * spacing, y * spacing, 0.f));
		}
	}

	//float k = 1000.f;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (x < width - 1)
				springs.emplace_back(&GetParticle(x, y), &GetParticle(x+1, y), k, spacing);

			if (y < height - 1)
				springs.emplace_back(&GetParticle(x, y), &GetParticle(x, y+1), k, spacing);

			if (x < width - 1 && y < height - 1)
				springs.emplace_back(&GetParticle(x, y), &GetParticle(x+1, y+1), k, spacing * sqrt(2.f));

			if (x < width - 1 && 0 < y)
				springs.emplace_back(&GetParticle(x, y), &GetParticle(x + 1, y - 1), k, spacing * sqrt(2.f));

			if (x < width - 2)
				springs.emplace_back(&GetParticle(x, y), &GetParticle(x + 2, y), k, spacing * 2);

			if(y < height - 2)
				springs.emplace_back(&GetParticle(x, y), &GetParticle(x, y + 2), k, spacing * 2);
		}
	}
}

void Cloth::Update(float deltaTime)
{
	for (auto& spring : springs)
		spring.applyForce();

	for (auto& particle : particles)
		particle.Update(deltaTime);
}

void Cloth::ApplyAcceleration(const glm::vec3& acceleration)
{
	for (auto& particle : particles)
		particle.AddForce(acceleration * particle.GetMass());
}

void Cloth::ApplyForceAtParticle(int x, int y, const glm::vec3& force)
{
	if (x >= 0 && x < particles.size() && y >= 0 && y < particles.size())
		particles[y * particles.size() + x].AddForce(force);
}

int Cloth::GetNumberOfParticles() const
{ 
	return particles.size();
}

glm::vec3 Cloth::GetPosition() const
{
	glm::vec3 centerOfMass(0.f);
	for (const auto& particle : particles)
		centerOfMass += particle.GetPosition();
	return centerOfMass / (float)GetNumberOfParticles();
}

void Cloth::SetPosition(const glm::vec3& position)
{
	glm::vec3 centerOfMass = GetPosition();

	glm::vec3 translation = position - centerOfMass;
	for (auto& particle : particles)
	{
		glm::vec3 newPosition = particle.GetPosition() + translation;
		particle.SetPosition(newPosition);
	}
}

glm::vec3 Cloth::GetVelocity() const
{
	glm::vec3 velocity(0.f);
	for (const auto& particle : particles)
		velocity += particle.GetVelocity();
	return velocity / (float)GetNumberOfParticles();
}

bool Cloth::HasParticle(const Particle* particle) const
{
	for (const Particle& p : particles) {
		if (&p == particle) return true;
	}
	return false;
}

Particle* Cloth::GetParticleAt(glm::vec3 position)
{
	float minDistance = glm::length(particles[0].GetPosition() - position);
	int index = 0;

	for (int i = 1; i < particles.size(); ++i)
	{
		float distance = glm::length(particles[i].GetPosition() - position);
		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}

	//if (minDistance > 0.5f * spacing) return 0;

	return &particles[index];
}

void SetClothModel(Model& model, const Cloth& cloth)
{
	Mesh* mesh = model.GetMesh();

	int numVerticesFace1 = mesh->vertices.size() / 2;

	//mesh->vertices.clear();

	int height = cloth.GetHeight();
	int width = cloth.GetWidth();

	for (int y = 0; y < height; ++y)
	for (int x = 0; x < width; ++x)
	{
		mesh->vertices[x + y * width].position = cloth.GetParticle(x, y).GetPosition();
		mesh->vertices[x + y * width].normal = glm::vec3(0.f);

		mesh->vertices[numVerticesFace1 + x + y * width].position = cloth.GetParticle(x, y).GetPosition();
		mesh->vertices[numVerticesFace1 + x + y * width].normal = glm::vec3(0.f);
	}

	// Calculate the normals
	for (int i = 0; i < mesh->GetNumTriangles(); ++i)
	{
		Triangle t = mesh->GetTriangle(i);
		glm::vec3 normal = -glm::normalize(glm::cross(t.b - t.a, t.c - t.a));

		mesh->vertices[mesh->indices[i * 3 + 0]].normal += normal;
		mesh->vertices[mesh->indices[i * 3 + 1]].normal += normal;
		mesh->vertices[mesh->indices[i * 3 + 2]].normal += normal;
	}

	// Normalize the normals
	for (int i = 0; i < mesh->GetNumVertices(); ++i)
	{
		mesh->vertices[i].normal = glm::normalize(mesh->vertices[i].normal);
	}

	// apply the changes
	mesh->Update();

	model.SetContent(mesh);
}

Cloth* ToCloth(PhysicsObject* object)
{
	if (object->GetType() == CLOTH)
		return static_cast<Cloth*>(object);

	return nullptr;
}