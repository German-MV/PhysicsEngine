#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (width / (float)height), nearPlane, farPlane);

	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	shader.SetUniformMat4(cameraMatrix, uniform);
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
	float deltaPosition = speed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += deltaPosition * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += deltaPosition * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += deltaPosition * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += deltaPosition * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += deltaPosition * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += deltaPosition * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 20.f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 10.f;
	}

	//bool orientationShouldChange = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

	static bool orientationShouldChange = false;
	static bool allowChange = true;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (allowChange == true)
			orientationShouldChange = !orientationShouldChange;

		allowChange = false;
	}
	else allowChange = true;


	if (orientationShouldChange)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = 100 * deltaTime * sensitivity * (float)(mouseY - (height / 2)) / 1000;
		float rotY = 100 * deltaTime * sensitivity * (float)(mouseX - (width / 2)) / 1000;

		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) or (glm::angle(newOrientation, -Up) <= glm::radians(5.0f))))
		{
			Orientation = newOrientation;
		}

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}

Frustum Camera::GetFrustum()
{
	Frustum result;

	glm::vec3 col1 = glm::vec3(cameraMatrix[0][0], cameraMatrix[1][0], cameraMatrix[2][0]);
	glm::vec3 col2 = glm::vec3(cameraMatrix[0][1], cameraMatrix[1][1], cameraMatrix[2][1]);
	glm::vec3 col3 = glm::vec3(cameraMatrix[0][2], cameraMatrix[1][2], cameraMatrix[2][2]);
	glm::vec3 col4 = glm::vec3(cameraMatrix[0][3], cameraMatrix[1][3], cameraMatrix[2][3]);

	result.left.normal = col4 + col1;
	result.right.normal = col4 - col1;
	result.bottom.normal = col4 + col2;
	result.top.normal = col4 - col2;
	result.near.normal = col4 + col3;
	result.far.normal = col4 - col3;

	result.left.distance	= cameraMatrix[3][3] + cameraMatrix[3][0];
	result.right.distance	= cameraMatrix[3][3] - cameraMatrix[3][0];
	result.bottom.distance	= cameraMatrix[3][3] + cameraMatrix[3][1];
	result.top.distance		= cameraMatrix[3][3] - cameraMatrix[3][1];
	result.near.distance	= cameraMatrix[3][3] + cameraMatrix[3][2];
	result.far.distance		= cameraMatrix[3][3] - cameraMatrix[3][2];

	for (int i = 0; i < 6; ++i)
	{
		float inverseNorm = 1.f / glm::length(result.planes[i].normal);
		result.planes[i].normal *= inverseNorm;
		result.planes[i].distance *= -inverseNorm;
	}

	return result;
}

Ray Camera::GetPickRay(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return ::GetPickRay((int)x, (int)y, width, height, cameraMatrix);
}