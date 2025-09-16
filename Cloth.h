#pragma once

#include "PhysicsObject.h"
#include "Particle.h"
#include "Spring.h"

#include <vector>

class Cloth : public PhysicsObject
{
protected:
	int width, height;
	float spacing;
	float k = 1000;

public:
	std::vector<Particle> particles;
	std::vector<Spring> springs;

	Cloth();
	Cloth(int width, int height, float spacing, float mass = 1.f);

	void Update(float deltaTime);

	void ApplyAcceleration(const glm::vec3& acceleration);

	void ApplyForceAtParticle(int x, int y, const glm::vec3& force);

	int GetNumberOfParticles() const;
	
	glm::vec3 GetPosition() const; // Returns the position of the center of mass of the cloth

	void SetPosition(const glm::vec3& position); // set position of the center of mass

	glm::vec3 GetVelocity() const;

	
	inline Particle& GetParticle(int x, int y)
	{
		return particles[y * width + x];
	}

	inline Particle GetParticle(int x, int y) const
	{
		return particles[y * width + x];
	}
	
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
	inline float GetSpacing() const { return spacing; }

	bool HasParticle(const Particle* particle) const;

	Particle* GetParticleAt(glm::vec3 position);
};

// Seria mas adecuado ponerlo en otro archivo
#include "Model.h"

void SetClothModel(Model& model, const Cloth& cloth);

Cloth* ToCloth(PhysicsObject* object);