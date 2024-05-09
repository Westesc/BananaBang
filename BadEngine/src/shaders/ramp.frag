#version 330 core

out vec4 FragColor;

in vec3 FragPos;

in vec3 Normal;

uniform vec3 lightPos;
uniform sampler2D gradientTexture;

void main()
{
	//vec3 lightDir = normalize(FragPos-lightPos);
	vec3 lightDir = normalize(lightPos-FragPos);
	vec3 color = texture(gradientTexture, vec2(dot(lightDir, Normal) * 0.5 + 0.5, 0.5)).rgb;
	FragColor = vec4(color, 1.0);
}