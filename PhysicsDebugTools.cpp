#include "PhysicsDebugTools.h"

void DrawRigidBody(
	RigidBody& rigidBody,
	Shader& shader,
	const char* uniformName
)
{
	// Position
	DrawPoint(rigidBody.GetPosition(), shader, uniformName, RED);

	// Velocity
	DrawVector(rigidBody.GetPosition(), rigidBody.GetPosition() + rigidBody.GetVelocity(), shader, uniformName, YELLOW);

	// Principal Inertia Axes
	glm::mat3 axes = glm::mat3_cast(rigidBody.GetOrientation());
	//glm::vec3 axesLength = glm::vec3(rigidBody.I1, rigidBody.I2, rigidBody.I3);
	glm::vec3 axesLength = glm::vec3(1.f);
	for (int i = 0; i < 3; ++i)
		DrawVector(rigidBody.GetPosition(), rigidBody.GetPosition() + axesLength[i] * axes[i], shader, uniformName, WHITE);

	// Angular Velocity
	DrawVector(rigidBody.GetPosition(), rigidBody.GetPosition() + rigidBody.GetAngularVelocity(), shader, uniformName, PURPLE);

	// Angular Momentum
	DrawVector(rigidBody.GetPosition(), rigidBody.GetPosition() + rigidBody.GetWorldAngularMomentum(), shader, uniformName, BLUE);
}

std::ostream& operator<<(std::ostream& os, RigidBody rigidBody)
{
	os << "Mass: " << rigidBody.GetMass() << std::endl;
	os << "Inertia tensor diagonal: " << rigidBody.GetI1() << ", " << rigidBody.GetI2() << ", " << rigidBody.GetI3() << std::endl;
	os << "Position: " << rigidBody.GetPosition() << std::endl;
	os << "Velocity: " << rigidBody.GetVelocity() << std::endl;
	os << "Angular Velocity: " << rigidBody.GetAngularVelocity() << std::endl;

	return os;
}