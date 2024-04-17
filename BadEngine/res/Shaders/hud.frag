#version 330 core
out vec4 color;

uniform vec3 col;
uniform float transparent;

void main()
{    
    color = vec4(col, transparent);
}