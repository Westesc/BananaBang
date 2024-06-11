#version 450 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
//layout (location = 3) in mat4 instanceMatrix;


out gl_PerVertex
{
	vec4 gl_Position;
};

out vec3 vertColor;

layout(location = 0) uniform mat4 M;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * M * vec4(inPos, 1.f);
	vertColor = vec3(1.0, 0.0, 0.0);
}