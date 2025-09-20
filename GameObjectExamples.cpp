#include "GameObjectExamples.h"
//#include "Rope.h"

#include "GeometrySamples.h"
#include "RigidBodySamples.h"
#include "RigidBodyCoordinator.h"
#include "ClothCoordinator.h"

#include "GameObjectSamples.h"

GameRigidBody CreateMergedCubesExample()
{
	GameRigidBody rigid1 = CreateGameCube(1.f, 1.f);
	GameRigidBody rigid2 = CreateGameCube(1.f, 1.f);

	rigid1->SetPosition(glm::vec3(1.f, 1.f, 10.f));
	rigid2->SetPosition(glm::vec3(0.f, 0.f, 10.f));
	
	GameRigidBody mergedRigids = MergeRigidBodies(rigid1, rigid2);
	mergedRigids->SetLocalAngularVelocity(10.f * glm::vec3(0.25f, 0.5f, 0.f));
	mergedRigids.SetColor(GREEN);

	return mergedRigids;
}

GameRigidBody CreateTHandleExample()
{

	float m = 1.f;
	float L = 1.f;
	float R = 0.5f;

	float mass = 2.f * m + m / 2.f;
	float cylinderLength1 = 2.f * L;
	float cylinderLength2 = L;
	float radius1 = R;
	float radius2 = 3 * R / 4.f;

	//RigidBody tHandle = CreateTHandle(mass, cylinderLength1, cylinderLength2, radius1, radius2);
	GameRigidBody tHandle(RigidBody(CreateTHandle(m, L, R)));
	//tHandle.orientation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));

	glm::vec3 angularVelocity = 10.f * glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 perturbation = glm::vec3(0.01f);

	tHandle->SetLocalAngularVelocity(angularVelocity + perturbation);

	glm::mat3 tHandleRotation = glm::mat3_cast(tHandle->GetOrientation());

	Mesh* cylinderMesh = CreateCylinderMesh(50, PURPLE);

	Model* cylinder1 = new Model(cylinderMesh);
	Model* cylinder2 = new Model(cylinderMesh);


	glm::vec3 cylinder1RelativePos = glm::vec3(0.f) - tHandleRotation[0] * (cylinderLength2 * cylinderLength2 / (cylinderLength1 + cylinderLength2));
	glm::vec3 cylinder2RelativePos = cylinder1RelativePos + tHandleRotation[0] * (0.5f * cylinderLength2);
	glm::quat cylinder1Orientation = glm::quat(glm::vec3(0.f, 1.f, 0.f), tHandleRotation[1]); //glm::angleAxis(glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	glm::quat cylinder2Orientation = glm::quat(glm::vec3(0.f, 1.f, 0.f), tHandleRotation[0]); //glm::angleAxis(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

	cylinder1->position = tHandle->GetPosition() + tHandle->GetOrientation() * cylinder1RelativePos;
	cylinder2->position = tHandle->GetPosition() + tHandle->GetOrientation() * cylinder2RelativePos;
	cylinder1->orientation = tHandle->GetOrientation() * cylinder1Orientation;
	cylinder2->orientation = tHandle->GetOrientation() * cylinder2Orientation;

	cylinder1->scale = glm::vec3(radius1, cylinderLength1, radius1);
	cylinder2->scale = glm::vec3(radius2, cylinderLength2, radius2);

	
	tHandle.AddModel(cylinder1);
	tHandle.AddModel(cylinder2);

	return tHandle;
}

GameRigidBody CreateMobilePhoneExample()
{
	glm::vec3 mobilePhoneSize = glm::vec3(3.f, 0.2f, 1.5f);
	float mass = 10.f;
	GameRigidBody mobilePhone = CreateGameBox(mass, mobilePhoneSize);
	mobilePhone->SetPosition(glm::vec3(0.f, 8.f, 0.f));

	return mobilePhone;
}

GameCloth CreateClothExample()
{
	int clothSize = 16;
	float clothLength = 1.f;
	float clothSpacing = clothLength / (float)clothSize;

	GameCloth cloth(clothSize, clothSize, clothSpacing);
	
	cloth->GetParticle(0, clothSize - 1).fixed = true;
	cloth->GetParticle(clothSize - 1, clothSize - 1).fixed = true;

	Mesh* clothMesh = cloth.GetMesh();

	for (int y = 0; y < cloth->GetHeight(); ++y)
		for (int x = 0; x < cloth->GetWidth(); ++x)
			clothMesh->vertices[x + y * cloth->GetWidth()].color = RED;

	for (int y = cloth->GetHeight() * 0.25f; y < cloth->GetHeight() * 0.75f; ++y)
		for (int x = 0.f; x < cloth->GetWidth(); ++x)
			clothMesh->vertices[x + y * cloth->GetWidth()].color = YELLOW;

	cloth.Coordinate();

	return cloth;
}