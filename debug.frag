#version 460 core
out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec4 color;

uniform vec3 camPos;

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 0.7f, 0.4f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return vec4(vec3(color) * (diffuse + ambient + specular), color.a);
}

void main()
{
	FragColor = direcLight();
}
