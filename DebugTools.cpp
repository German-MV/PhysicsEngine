#include "DebugTools.h"

std::ostream& operator<<(std::ostream& os, glm::vec3 vec)
{
	os << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, glm::mat3 mat)
{
	os << "[" << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << "]\n" <<
		"[" << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << "]\n" <<
		"[" << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << "]" << std::endl;

	return os;
}

std::ostream& operator<<(std::ostream& os, glm::mat4 mat)
{
	os << "[" << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << "]\n" <<
		"[" << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << "]\n" <<
		"[" << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << "]\n" <<
		"[" << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << "]" << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, Plane plane)
{
	os << "Plane: " << plane.normal << " " << plane.distance;
	return os;
}

std::ostream& operator<<(std::ostream& os, Frustum frustum)
{
	os << frustum.bottom << std::endl;
	os << frustum.top << std::endl;
	os << frustum.left << std::endl;
	os << frustum.right << std::endl;
	os << frustum.near << std::endl;
	os << frustum.far;

	return os;
}