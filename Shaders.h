#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <string>

#include <glm/glm.hpp>

std::string getFileContents(const char* filename);

class ShaderModule
{
public:
	GLuint ID;

	ShaderModule() = default;
	ShaderModule(const std::string& code, GLenum shaderType);
	~ShaderModule();

	void Delete();

	bool isValid() const { return ID != 0; }

	inline GLuint getID() const { return ID; }
};

ShaderModule FragmentShader(const std::string& code);

ShaderModule VertexShader(const std::string& code);

ShaderModule ComputeShader(const std::string& code);

class ShaderProgram
{
public:
	GLuint ID;

	ShaderProgram();
	~ShaderProgram();

	void AttachShader(const ShaderModule& shader);

	void Link();

	void Activate();
	void Delete();

	bool isValid() const { return ID != 0; }

	inline GLuint getID() const { return ID; }

	int getUniformLocation(const char* uniform);

	void SetUniformFloat(float value, const char* uniform);
	void SetUniformInt(int value, const char* uniform);
	void SetUniformUInt(unsigned int value, const char* uniform);

#ifdef GLM_VERSION
	void SetUniformVec2(const glm::vec2& vector, const char* uniform);
	void SetUniformVec3(const glm::vec3& vector, const char* uniform);
	void SetUniformVec4(const glm::vec4& vector, const char* uniform);

	void SetUniformIVec2(const glm::ivec2& vector, const char* uniform);
	void SetUniformIVec3(const glm::ivec3& vector, const char* uniform);
	void SetUniformIVec4(const glm::ivec4& vector, const char* uniform);

	void SetUniformUVec2(const glm::uvec2& vector, const char* uniform);
	void SetUniformUVec3(const glm::uvec3& vector, const char* uniform);
	void SetUniformUVec4(const glm::uvec4& vector, const char* uniform);

	void SetUniformMat2(const glm::mat2& matrix, const char* uniform);
	void SetUniformMat3(const glm::mat3& matrix, const char* uniform);
	void SetUniformMat4(const glm::mat4& matrix, const char* uniform);
#endif
};

ShaderProgram CreateShaderFromFiles(const char* vertexFile, const char* fragmentFile);


class ComputeProgram : public ShaderProgram
{
public:
	ComputeProgram(const std::string& code);
	~ComputeProgram();

	void Dispatch(int x, int y, int z);
};

typedef ShaderProgram Shader;

#endif