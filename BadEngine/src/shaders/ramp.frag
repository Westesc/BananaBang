#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightPos;
uniform sampler2D gradientTexture;

void main()
{
    float distance = length(lightPos - FragPos);
    float uv = distance / 64;
    uv = clamp(uv, 0.1, 0.8);
    vec3 color = texture(gradientTexture, vec2(uv, 1.0)).rgb;
    FragColor = vec4(color, 1.0);
}