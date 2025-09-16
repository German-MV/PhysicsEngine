#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;

out vec3 crntPos;
out vec3 Normal;
out vec4 color;

uniform mat4 camMatrix;
uniform mat4 model;

uniform vec3 planeNormal;
uniform float planeDist;

void main()
{
	vec3 pos = (model * vec4(aPos, 1.0f)).xyz;
	float dist = dot(pos, planeNormal) - planeDist;
	vec3 projectedPos = pos - planeNormal * dist;

	crntPos = vec3(model * vec4(projectedPos, 1.f));
	Normal = normalize(vec3(model * vec4(aNormal, 0.f)));
	color = aColor;

	gl_Position = camMatrix * vec4(projectedPos, 1.0f);
}