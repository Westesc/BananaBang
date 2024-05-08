#version 330 core

out vec4 FragColor;

in vec3 FragPos;

in vec3 Normal;

uniform vec3 lightPos;
uniform sampler2D gradientTexture;

void main()
{
	vec3 lightDir = normalize(FragPos-lightPos);
	//vec3 lightDir = normalize(lightPos-FragPos);
	vec3 normal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
	vec3 color = texture(gradientTexture, vec2(clamp(dot(lightDir, Normal) * 0.5 + 0.5, 0.1, 0.8), 0.0)).rgb;
	FragColor = vec4(color, 1.0);
}