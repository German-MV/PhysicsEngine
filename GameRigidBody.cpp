#include "GameRigidBody.h"

GameRigidBody MergeRigidBodies(GameRigidBody& a, GameRigidBody& b)
{
	RigidBody* mergedRigids = new RigidBody(MergeRigidBodies(*a.GetPhysics(), *b.GetPhysics()));

	GameRigidBody mergedGameRigids(*mergedRigids);

	a.Coordinate();
	b.Coordinate();

	for (Model* model : a.GetModels())
	{
		mergedGameRigids.AddModel(model);
	}

	for (Model* model : b.GetModels())
	{
		mergedGameRigids.AddModel(model);
	}

	return mergedGameRigids;
}