#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightPos;
uniform sampler2D gradientTexture;

void main()
{
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 normal = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
	vec3 color = texture(gradientTexture, vec2(clamp(dot(lightDir, normal) * 0.5 + 0.5, 0.1, 1.0), 0.0)).rgb;
	FragColor = vec4(color, 1.0);
}