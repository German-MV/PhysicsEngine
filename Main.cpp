#include <iostream>
#include <iomanip>

#include "GMV_Physics.h"
#include "GMV_Samples.h"

#include "DebugTools.h"
#include "PhysicsDebugTools.h"

#include "GameObjectExamples.h"

unsigned int width = 1920;
unsigned int height = 1080;

const glm::vec3 gravity = glm::vec3(0.f, -9.81f, 0.f);

int main()
{
	// Debug
	std::cout << std::setprecision(3);



	// ############ OPENGL SETUP ############ 
	
	// GLFW Inicialization
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4); // Antialiasing

	// Window creation
	GLFWwindow* window = glfwCreateWindow(width, height, "Physics Simulation", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);


	// Load GLAD to configure OpenGL
	gladLoadGL();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, width, height);

	glfwSetWindowSizeCallback(
		window,
		[](GLFWwindow* window, int _width, int _height)
		{
			glViewport(0, 0, _width, _height);
			width = _width;
			height = _height;
		}
	);

	glEnable(GL_MULTISAMPLE); // Antialiasing

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	// ############ RENDERING SETUP ############
	 
	// SHADERS
	// General shader
	Shader shaderProgram = CreateShaderFromFiles("debug.vert", "debug.frag");
	if (!shaderProgram.isValid())
	{
		std::cout << "Failed to create shader program" << std::endl;
		return -1;
	}
	shaderProgram.Activate();

	// Projection shader for the cursor
	Shader projectionShader = CreateShaderFromFiles("projection.vert", "projection.frag");
	if (!projectionShader.isValid())
	{
		std::cout << "Failed to create shader program" << std::endl;
		return -1;
	}
	projectionShader.Activate();
	
	// CAMERA
	glm::vec3 camPos = glm::vec3(0.5f, 0.7f, 13.f);
	Camera camera(width, height, camPos);
	camera.nearPlane = 0.1f;
	camera.farPlane = 1000.f;
	camera.updateMatrix();

	Frustum camFrustum;



	// ############ ENGINE SETUP ############

	Engine engine;
	Cursor cursor(engine, camera, window);



	// MODELS

	Mesh* sphereMesh = CreateSphereMesh(50, 50, ORANGE);
	Mesh* cylinderMesh = CreateCylinderMesh(50, PURPLE);
	Mesh* coneMesh = CreateConeMesh(50, CYAN);
	Mesh* cubeMesh = CreateCubeMesh(GREEN);

	Model sphere(sphereMesh);
	Model cylinder(cylinderMesh);
	Model cone(coneMesh);
	Model cube(cubeMesh);

	cube.position = glm::vec3(-3.f, 0.f, 0.f);
	sphere.position = glm::vec3(-1.f, 0.f, 0.f);
	cylinder.position = glm::vec3(1.f, 0.f, 0.f);
	cone.position = glm::vec3(3.f, 0.f, 0.f);
	
	cylinder.scale = glm::vec3(1.f, 3.f, 1.f);
	cylinder.orientation = glm::quat(glm::vec3(0.f, 0.f, glm::radians(45.f)));

	engine.AddModel(&sphere);
	engine.AddModel(&cylinder);
	engine.AddModel(&cone);
	engine.AddModel(&cube);



	// T-HANDLE

	GameRigidBody tHandle = CreateTHandleExample();
	tHandle->SetPosition(glm::vec3(-2.f, 3.f, 0.f));
	engine.AddObject(&tHandle);
	RigidBody* tHandlePhysics = tHandle.GetPhysics();
	//GameApplicationPoint tHandlePoint(tHandle, tHandle.GetModels()[1]->position + glm::vec3(0.5f, 0.f, 0.f), 0.1f);
	//engine.AddObject(&tHandlePoint);



	// MOBILE PHONE

	glm::vec3 mobilePhoneSize = glm::vec3(3.f, 0.2f, 1.5f);
	GameRigidBody mobilePhone = CreateGameBox(10.f, mobilePhoneSize);
	engine.AddObject(&mobilePhone);
	mobilePhone->SetPosition(glm::vec3(0.f, 5.f, 0.f));
	mobilePhone->SetDamping(5.f);
	mobilePhone->SetAngularDamping(1.f);
	mobilePhone.SetColor(GREEN);

	GameApplicationPoint mobilePhonePoint(mobilePhone, mobilePhone->GetOrientation() * (mobilePhoneSize / 2.f + mobilePhone->GetPosition()), 0.1f);
	GameParticle particle_mobilePhone(0.2f, 1.f);
	engine.AddObject(&mobilePhonePoint);
	engine.AddObject(&particle_mobilePhone);

	particle_mobilePhone->fixed = true;
	particle_mobilePhone->SetPosition(mobilePhone->GetPosition() + mobilePhoneSize);

	GameSpring spring_mobilePhone(particle_mobilePhone, mobilePhonePoint, 1000.f, 0.f);
	engine.AddObject(&spring_mobilePhone);
	spring_mobilePhone->SetDamping(spring_mobilePhone->GetConstant() / 50.f);



	// CLOTH

	int clothSize = 16;
	float clothLength = 1.f;
	float clothSpacing = clothLength / (float) clothSize;

	GameCloth cloth(clothSize, clothSize, clothSpacing, 1.f);
	engine.AddObject(&cloth);
	cloth->GetParticle(0, clothSize - 1).fixed = true;
	cloth->GetParticle(clothSize - 1, clothSize - 1).fixed = true;	
	cloth->SetPosition(glm::vec3(5.f, 0.f, 0.f));

	GameRigidBody boxInCloth = CreateGameBox(100, glm::vec3(1.f));
	boxInCloth->SetPosition(cloth->GetParticle(clothSize - 1, 0).GetPosition() - glm::vec3(0.5f));

	GameApplicationPoint appBoxInCloth1(boxInCloth, boxInCloth->GetPosition() + glm::vec3(0.5f, 0.5f, 0.5f), 0.1f);
	GameApplicationPoint appCloth1(cloth, cloth->GetParticle(clothSize - 1, 0).GetPosition());
	GameSpring springBoxInCloth1(appBoxInCloth1, appCloth1, 10000.f, 0.f);
	springBoxInCloth1->SetDamping(springBoxInCloth1->GetConstant() / 100.f);

	GameApplicationPoint appBoxInCloth2(boxInCloth, boxInCloth->GetPosition() + glm::vec3(-0.5f, 0.5f, 0.5f), 0.1f);
	GameApplicationPoint appCloth2(cloth, cloth->GetParticle(0, 0).GetPosition());
	GameSpring springBoxInCloth2(appBoxInCloth2, appCloth2, 10000.f, 0.f);
	springBoxInCloth2->SetDamping(springBoxInCloth2->GetConstant() / 100.f);

	engine.AddObject(&boxInCloth);
	engine.AddObject(&springBoxInCloth1);
	engine.AddObject(&springBoxInCloth2);

	// Color the cloth with the flag of Poland
	Mesh* clothMesh = cloth.GetMesh();

	for (int y = 0; y < cloth->GetHeight()/2; ++y)
		for (int x = 0; x < cloth->GetWidth(); ++x)
			clothMesh->vertices[x + y * cloth->GetWidth()].color = RED;
	


	// SIMPLE TWO PARTICLE + SPRING SYSTEM

	float particleRadius = 0.1f;

	GameParticle particle1(particleRadius, 1.f);
	GameParticle particle2(particleRadius, 1.f);

	particle1->SetPosition(glm::vec3(3.5f, -3.f, 0.f));
	particle2->SetPosition(particle1->GetPosition() - glm::vec3(7.f, 0.f, 0.f));

	engine.AddObject(&particle1);
	engine.AddObject(&particle2);

	GameSpring spring(particle1, particle2, 100.f, 2.f);
	engine.AddObject(&spring);



	// CHAIN OF CYLINDERS

	constexpr int numCylinders = 10;
	constexpr int numSprings = numCylinders - 1;

	GameRigidBody rigids[numCylinders];
	GameApplicationPoint appPoints[numSprings*2];
	
	float cylinderHeight = 0.5f;
	glm::vec3 dir = glm::vec3(0.f, -cylinderHeight, 0.f);
	
	for (int i = 0; i < numCylinders; ++i)
	{
		rigids[i] = CreateGameCylinder(10.f, 0.2f, cylinderHeight);
		rigids[i]->SetPosition(glm::vec3(-5.f, 5.f, 0.f) + dir * (float)i);
		rigids[i]->SetDamping(1.f);
		rigids[i]->SetAngularDamping(1.f);
		engine.AddObject(&rigids[i]);
	}
	
	for(int i = 0; i < numCylinders; ++i)
	{
		if (i != 0) appPoints[2*i-1] = GameApplicationPoint(rigids[i], rigids[i]->GetPosition() - dir * 0.5f, 0.1f);
		if (i != numCylinders-1) appPoints[2*i] = GameApplicationPoint(rigids[i], rigids[i]->GetPosition() + dir * 0.5f, 0.1f);
	}

	GameSpring springs[numSprings];
	for (int i = 0; i < numSprings; ++i)
	{
		springs[i] = GameSpring(appPoints[2 * i], appPoints[2 * i + 1], 2000.f, 0.f);
		springs[i]->SetDamping(springs[i]->GetConstant() / 50.f);
		engine.AddObject(&springs[i]);
	}

	GameRigidBody& topRigid = rigids[0];
	GameParticle topParticle(0.2f, 1.f);
	topParticle->fixed = true;
	glm::vec3 perturbation = glm::vec3(
		((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f,
		((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f,
		((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f
	);
	topParticle->SetPosition(topRigid->GetPosition() + dir * 0.5f + perturbation);
	engine.AddObject(&topParticle);
	GameApplicationPoint topAppPoint(topRigid, topRigid->GetPosition() - dir * 0.5f, 0.1f);
	GameSpring topSpring(topParticle, topAppPoint, 2000.f, 0.f);
	topSpring->SetDamping(topSpring->GetConstant() / 50.f);
	engine.AddObject(&topSpring);


	// ############ MAIN LOOP ############
	
	// MAIN LOOP SETUP
	const float fpsLimit = 120.f;
	float currentTime = glfwGetTime();
	float deltaTime = 1 / fpsLimit;
	float minimumDeltaTime = 1 / 100.f; // Minimum delta time to avoid too high speeds
	float simulationSpeed = 1.f;

	// LOOP
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Cursor management
		cursor.handleCursor();
		if (!cursor.isActive() && 
			(
				glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GL_TRUE ||
				glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GL_TRUE
			)
		)
		{
			simulationSpeed = 0.3f;
		}
		else simulationSpeed = 1.f;

		// Update all objects
		cylinder.orientation = glm::rotate(cylinder.orientation, deltaTime, glm::vec3(0.f, 1.f, 0.f));
		cylinder.orientation = glm::rotate(glm::quat(1.f, 0.f, 0.f, 0.f), deltaTime, glm::vec3(0.f, 1.f, 0.f)) * cylinder.orientation;

		cloth->ApplyAcceleration(gravity);
		mobilePhone->ApplyAcceleration(gravity);
		boxInCloth->ApplyAcceleration(gravity);
		for(int i = 0; i < numCylinders; ++i)
		{
			rigids[i]->ApplyAcceleration(gravity);
		}

		engine.Update(deltaTime * simulationSpeed);

		// Rendering
		// Shader and camera update
		shaderProgram.Activate();

		camera.Inputs(window, deltaTime);
		camera.updateMatrix();
		camera.Matrix(shaderProgram, "camMatrix");
		camFrustum = camera.GetFrustum();
		shaderProgram.SetUniformVec3(camera.Position, "camPos");

		engine.Render(camFrustum, shaderProgram, "model");
		//DrawRigidBody(*tHandlePhysics, shaderProgram, "model");
		//DrawRigidBody(*mobilePhone.GetPhysics(), shaderProgram, "model");

		cursor.renderCursor(shaderProgram, "model");


		// Wait until next photogram should happen and get deltaTime (do not separate this lines)
		{
			while ((float)glfwGetTime() - currentTime < 1 / fpsLimit);
			deltaTime = (float)glfwGetTime() - currentTime;
			currentTime = (float)glfwGetTime();

			if (deltaTime > minimumDeltaTime / simulationSpeed) deltaTime = minimumDeltaTime / simulationSpeed;
		}


		glfwSetWindowTitle(window, std::to_string(1.f / deltaTime).c_str());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//DeleteGameObjects();

	//DeleteGeometrySamples();

	glDisable(GL_BLEND);

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}