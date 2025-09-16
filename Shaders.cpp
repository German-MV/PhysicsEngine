#include "Shaders.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

// Reads a text file and outputs a string with everything in the text file
std::string getFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

void compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}


ShaderModule::ShaderModule(const std::string& code, GLenum shaderType)
{
	const char* source = code.c_str();

	ID = glCreateShader(shaderType);
	glShaderSource(ID, 1, &source, NULL);
	glCompileShader(ID);

	switch (shaderType)
	{
	case GL_VERTEX_SHADER:
		compileErrors(ID, "VERTEX");
		break;
	case GL_FRAGMENT_SHADER:
		compileErrors(ID, "FRAGMENT");
		break;
	case GL_COMPUTE_SHADER:
		compileErrors(ID, "COMPUTE");
		break;
	}
}

ShaderModule::~ShaderModule()
{
	Delete();
}

void ShaderModule::Delete()
{
	if (!isValid()) return;

	glDeleteShader(ID);
	ID = 0;
}


ShaderModule FragmentShader(const std::string& code)
{
	return ShaderModule(code, GL_FRAGMENT_SHADER);
}

ShaderModule VertexShader(const std::string& code)
{
	return ShaderModule(code, GL_VERTEX_SHADER);
}

ShaderModule ComputeShader(const std::string& code)
{
	return ShaderModule(code, GL_COMPUTE_SHADER);
}


ShaderProgram::ShaderProgram()
{
	ID = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
	Delete();
}

void ShaderProgram::AttachShader(const ShaderModule& shader)
{
	glAttachShader(ID, shader.getID());
}

void ShaderProgram::Link()
{
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");
}

void ShaderProgram::Activate()
{
	glUseProgram(ID);
}

void ShaderProgram::Delete()
{
	if (!isValid()) return;

	glDeleteProgram(ID);
	ID = 0;
}

int ShaderProgram::getUniformLocation(const char* uniform)
{
	GLint location = glGetUniformLocation(ID, uniform);

	if (location == -1)
		std::cout << "Uniform " << uniform << " not found in shader" << std::endl;

	return location;
}

// UNIFORMS

// float, int, uint

void ShaderProgram::SetUniformFloat(float value, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform1f(location, value);
}

void ShaderProgram::SetUniformInt(int value, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform1i(location, value);
}

void ShaderProgram::SetUniformUInt(unsigned int value, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform1ui(location, value);
}

#ifdef GLM_VERSION
#include <glm/gtc/type_ptr.hpp>

// glm::vec2, glm::vec3, glm::vec4

void ShaderProgram::SetUniformVec2(const glm::vec2& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform2f(location, vector.x, vector.y);
}

void ShaderProgram::SetUniformVec3(const glm::vec3& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::SetUniformVec4(const glm::vec4& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

// glm::ivec2, glm::ivec3, glm::ivec4

void ShaderProgram::SetUniformIVec2(const glm::ivec2& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform2i(location, vector.x, vector.y);
}

void ShaderProgram::SetUniformIVec3(const glm::ivec3& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform3i(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::SetUniformIVec4(const glm::ivec4& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform4i(location, vector.x, vector.y, vector.z, vector.w);
}

// glm::uvec2, glm::uvec3, glm::uvec4

void ShaderProgram::SetUniformUVec2(const glm::uvec2& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform2ui(location, vector.x, vector.y);
}

void ShaderProgram::SetUniformUVec3(const glm::uvec3& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform3ui(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::SetUniformUVec4(const glm::uvec4& vector, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniform4ui(location, vector.x, vector.y, vector.z, vector.w);
}

// glm::mat2, glm::mat3, glm::mat4

void ShaderProgram::SetUniformMat2(const glm::mat2& matrix, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::SetUniformMat3(const glm::mat3& matrix, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::SetUniformMat4(const glm::mat4& matrix, const char* uniform)
{
	GLint location = getUniformLocation(uniform);
	if (location == -1) return;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

#endif

ShaderProgram CreateShaderFromFiles(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);

	ShaderModule vertexShader = VertexShader(vertexCode);
	ShaderModule fragmentShader = FragmentShader(fragmentCode);

	ShaderProgram shaderProgram;
	shaderProgram.AttachShader(vertexShader);
	shaderProgram.AttachShader(fragmentShader);
	shaderProgram.Link();

	vertexShader.Delete();
	fragmentShader.Delete();

	return shaderProgram;
}

ComputeProgram::ComputeProgram(const std::string& code)
{
	ID = glCreateProgram();

	ShaderModule shader = ComputeShader(code);

	this->AttachShader(shader);

	this->Link();

	shader.Delete();
}

ComputeProgram::~ComputeProgram()
{
	Delete();
}

void ComputeProgram::Dispatch(int x, int y, int z)
{
	glDispatchCompute(x, y, z);
}