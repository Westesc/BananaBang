#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 M;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineScale;

void main()
{
	vec3 crntPos = vec3(M * vec4(aPos + aNormal * outlineScale, 1.0f));
	gl_Position = M * projection * view * vec4(crntPos, 1.0);
}