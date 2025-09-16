#pragma once

#include <iostream>
#include <vector>
#include "Geometry3D.h"

#define CHECK std::cout << "check" << std::endl;

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vector)
{
	for (auto& elem : vector)
		os << elem << " ";
	os << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, glm::vec3 vec);

std::ostream& operator<<(std::ostream& os, glm::mat3 mat);

std::ostream& operator<<(std::ostream& os, glm::mat4 mat);

std::ostream& operator<<(std::ostream& os, Plane plane);

std::ostream& operator<<(std::ostream& os, Frustum frustum);