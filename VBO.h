#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class VBO
{
public:
	GLuint ID;

	VBO() = default;
	
	template <typename V>
	VBO(std::vector<V>& vertices, GLenum usage = GL_STATIC_DRAW)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(V), vertices.data(), usage);
	}

	template <typename V>
	void Update(std::vector<V> vertices)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(V), vertices.data());
		Unbind();
	}

	void Bind();
	void Unbind();
	void Delete();
};

#endif