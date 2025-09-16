#pragma once

#include "RigidBody.h"
#include "GeometrySamples.h"
#include "DebugTools.h"

void DrawRigidBody(
	RigidBody& rigidBody,
	Shader& shader,
	const char* uniformName
);

std::ostream& operator<<(std::ostream& os, RigidBody rigidBody);