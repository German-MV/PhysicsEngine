#pragma once

#include "ApplicationPoint.h"
//#include "Particle.h"
//#include "RigidBodyPoint.h"

class Spring : public PhysicsObject
{
protected:
	ApplicationPoint* p1;
	ApplicationPoint* p2;

	float k;
	float restingLength;
	float damping = 1.f;

public:

	Spring() : PhysicsObject(SPRING) {}

	Spring(ApplicationPoint* point1, ApplicationPoint* point2, float k = 1.f, float restingLength = 0.f)
		: p1(point1), p2(point2), k(k), restingLength(restingLength), PhysicsObject(SPRING)
	{}

	void applyForce();

	inline void SetConstant(float newK) { k = newK; }
	inline void SetRestingLength(float newRestingLength) { restingLength = newRestingLength; }
	inline void SetDamping(float newDamping) { damping = newDamping; }
	
	inline float GetConstant() const { return k; }
	inline float GetRestingLength() const { return restingLength; }
	inline float GetDamping() const { return damping; }

	inline ApplicationPoint* GetPoint1() const { return p1; }
	inline ApplicationPoint* GetPoint2() const { return p2; }

//private:
//	template<typename T1, typename T2>
//	void computeForce(T1* p1, T2* p2);
	friend class PhysicsSystem;
	friend glm::vec3 GetSpringRelativePositions(const Spring& spring);
	friend glm::vec3 GetSpringCenter(const Spring& spring);
};

glm::vec3 GetSpringRelativePositions(const Spring& spring);

glm::vec3 GetSpringCenter(const Spring& spring);

Spring* ToSpring(PhysicsObject* object);