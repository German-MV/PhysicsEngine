#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;

out vec3 crntPos;
out vec3 Normal;
out vec4 color;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
	crntPos = vec3(model * vec4(aPos, 1.f));
	Normal = normalize(vec3(model * vec4(aNormal, 0.f)));
	color = aColor;

	gl_Position = camMatrix * model * vec4(aPos, 1.0f);
}