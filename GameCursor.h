#pragma once

#include "Engine.h"
#include "GameSelection.h"
#include "Camera.h"

#include "GLFW/glfw3.h"

enum {
	NONE,
	BOTH,
	LEFT,
	RIGHT
};

class Cursor
{
private:
	Camera& camera;
	Engine& engine;
	GLFWwindow* window;

	Ray selectionRay;
	GameSelection selection;

	Plane movementPlane;

	bool lockedSelection;
	int lastSelectionMode;

	glm::vec3 cursorPosition;
	glm::vec3 selectionInitialPosition;
	glm::vec3 offset;

	float cooldown = 0.f;
	float cooldownTimer = 0.f;
	float lastClickTime = 0.f;

	glm::vec3 GetSelectionPoint()
	{
		float t = Raycast(movementPlane, selectionRay);
		if (t < 0.f) // If the ray does not intersect the plane, invert the plane and try again
		{
			movementPlane.distance = -movementPlane.distance;
			movementPlane.normal = -movementPlane.normal;
			t = Raycast(movementPlane, selectionRay);
		}

		if (t < 0.f) return selectionInitialPosition;

		return selectionRay.origin + t * selectionRay.direction;
	}

	Model* selectedModel;

	void HandleModels()
	{
		//std::cout << "Handling models" << std::endl;

		if (!lockedSelection)
		{
			selectedModel = selection.object->GetModels().front();
			offset = selection.point - selectedModel->position;
		}
		
		selectedModel->position = cursorPosition - offset;
	}

	bool particleInitialFix;
	Particle* selectedParticle;

	void HandleParticle()
	{
		//std::cout << "Handling particle" << std::endl;

		if (!lockedSelection)
		{
			selectedModel = selection.object->GetModels().front();
			selectedParticle = ToParticle(selection.object->GetPhysicsObject());
			offset = selection.point - selectedParticle->GetPosition();
			particleInitialFix = selectedParticle->fixed;
			selectedParticle->fixed = true;
		}

		selectedParticle->SetPosition(cursorPosition - offset);
	}

	void HandleCloth()
	{
		//std::cout << "Handling cloth" << std::endl;

		if(!lockedSelection)
		{
			selectedModel = selection.object->GetModels().front();
			selectedParticle = ToCloth(selection.object->GetPhysicsObject())->GetParticleAt(selection.point);
			offset = selection.point - selectedParticle->GetPosition();
			particleInitialFix = selectedParticle->fixed;
			selectedParticle->fixed = true;
		}

		selectedParticle->SetPosition(cursorPosition - offset);
	}

	GameApplicationPoint* selectedRigidPoint = nullptr;
	GameSpring* selectionSpring = nullptr;
	GameParticle* cursorParticle = nullptr;

	void HandleRigidBody()
	{
		//std::cout << "Handling rigid body" << std::endl;
		if (!lockedSelection)
		{
			cursorParticle = new GameParticle(0.2f, 1.f);
			(*cursorParticle)->SetPosition(selection.point);
			(*cursorParticle)->fixed = true;
			selectedRigidPoint = new GameApplicationPoint(*static_cast<GameRigidBody*>(selection.object), selection.point, 0.f);
			selectionSpring = new GameSpring(*selectedRigidPoint, *cursorParticle, 1000.f, 0.f);
			(*selectionSpring)->SetDamping((*selectionSpring)->GetConstant() / 50.f);

			selectedModel = cursorParticle->GetModels().front();


			engine.AddObject(cursorParticle);
			//engine.AddObject(selectedRigidPoint);
			engine.AddObject(selectionSpring);
		}

		(*cursorParticle)->SetPosition(cursorPosition);
	}

	ApplicationPoint* selectedApplicationPoint;

	void HandleApplicationPoint()
	{
		//std::cout << "Handling application point" << std::endl;

		if(!lockedSelection)
		{
			selectedApplicationPoint = ToApplicationPoint(selection.object->GetPhysicsObject());
			offset = selection.point - selectedParticle->GetPosition();
		}

		//selectedApplicationPoint->SetPosition(cursorPosition - offset);
	}

	Shader projectionShader;

public:
	Cursor(Engine& engine, Camera& camera, GLFWwindow* window) : 
		engine(engine), camera(camera), window(window), 
		projectionShader(CreateShaderFromFiles("projection.vert", "projection.frag"))
	{
		if(!projectionShader.isValid())
		{
			std::cerr << "Failed to create projection shader" << std::endl;
		}
		projectionShader.Activate();

		selectedParticle = nullptr;
		resetCursor();

	}

	void resetCursor()
	{
		lockedSelection = false;
		selectedModel = nullptr;
		lastSelectionMode = NONE;

		selectionInitialPosition = glm::vec3(0.f);
		offset = glm::vec3(0.f);

		if(selectedParticle != nullptr)
		{
			selectedParticle->fixed = particleInitialFix;
			selectedParticle = nullptr;
		}

		// Rigid body selection cleanup
		if (selectionSpring != nullptr)
		{
			engine.RemoveGameObject(selectionSpring);
			delete selectionSpring;
			selectionSpring = nullptr;
		}
		if (cursorParticle != nullptr)
		{
			engine.RemoveGameObject(cursorParticle);
			delete cursorParticle;
			cursorParticle = nullptr;
		}
		if (selectedRigidPoint != nullptr)
		{
			//engine.RemoveGameObject(selectedRigidPoint);
			delete selectedRigidPoint;
			selectedRigidPoint = nullptr;
		}
	}

	void handleCursor()
	{
		bool leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		bool rightMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		
		if(!leftMousePressed && !rightMousePressed)
		{
			resetCursor();
			return;
		}
			
		/*if(!lockedSelection)
		{
			cooldownTimer = glfwGetTime();
			if (cooldownTimer - lastClickTime < cooldown)
			{
				return;
			}
			lastClickTime = cooldownTimer;
		}*/

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		selectionRay = GetPickRay(mouseX, mouseY, camera.width, camera.height, camera.cameraMatrix);

		if(!lockedSelection)
		{
			// update cloths accelerators
			for(GameObject* object : engine.objects)
			{
				if(object->GetPhysicsObject() != nullptr && object->GetType() == CLOTH)
				{
					object->GetModels()[0]->GetMesh()->UpdateAccelerator();
				}
			}

			selection = engine.Raycast(selectionRay);
			cursorPosition = selection.point;

			const float maxDistance = 100000.f;
			if(selection.object != nullptr && glm::length(selection.point - selectionRay.origin) > maxDistance)
			{
				resetCursor();
				return;
			}
		}
		
		if (selection.object == nullptr) return;

		// now we want to find the new position of the object, contained in one of the planes xy, xz or yz.
		if (leftMousePressed && rightMousePressed) // both buttons pressed
		{
			if (!lockedSelection || lastSelectionMode != BOTH)
				movementPlane = Plane(glm::vec3(0.f, 1.f, 0.f), cursorPosition.y);
			lastSelectionMode = BOTH;
		}
		else if (leftMousePressed) // left button pressed
		{
			if (!lockedSelection || lastSelectionMode != LEFT)
				movementPlane = Plane(glm::vec3(0.f, 0.f, 1.f), cursorPosition.z);
			lastSelectionMode = LEFT;
		}
		else if (rightMousePressed) // right button pressed
		{
			if (!lockedSelection || lastSelectionMode != RIGHT)
				movementPlane = Plane(glm::vec3(1.f, 0.f, 0.f), cursorPosition.x);
			lastSelectionMode = RIGHT;
		}

		cursorPosition = GetSelectionPoint();

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			particleInitialFix = true;
			cursorParticle = nullptr;
			selectedRigidPoint = nullptr;
			selectionSpring = nullptr;
			resetCursor();
			return;
		}
		//else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		//{
		//	//std::cout << "Removed object: " << selection.object->GetType() << std::endl;
		//	engine.RemoveGameObject(selection.object);
		//	resetCursor();
		//}

		switch (selection.GetType())
		{
		case NO_PHYSICS:
			HandleModels();
			break;
		case PARTICLE:
			HandleParticle();
			break;
		case CLOTH:
			HandleCloth();
			break;
		case RIGID_BODY:
			HandleRigidBody();
			break;
		/*case APPLICATION_POINT:
			HandleApplicationPoint();
			break;*/
		default:
			resetCursor();
			return;
		}

		lockedSelection = true;
	}

	bool isActive() const
	{
		return lockedSelection;
	}

	void renderCursor(Shader& shader, const char* uniformName)
	{
		if (selectedModel != nullptr)
		{
			DrawPlane(movementPlane, shader, uniformName);

			projectionShader.Activate();
			camera.Matrix(projectionShader, "camMatrix");
			projectionShader.SetUniformVec3(camera.Position, "camPos");

			projectionShader.SetUniformVec3(movementPlane.normal, "planeNormal");
			projectionShader.SetUniformFloat(movementPlane.distance, "planeDist");

			glDepthMask(GL_FALSE); // Deshabilita escritura en el Z-buffer para evitar artefactos
			for (Model*& model : engine.scene.objects)
				model->Render(projectionShader, "model");
			glDepthMask(GL_TRUE); // Reactiva escritura en el Z-buffer
		}
	}
};