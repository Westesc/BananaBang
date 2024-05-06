#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightPos;
uniform sampler1D gradientTexture;

void main()
{
    float distance = length(lightPos - FragPos);
    float uv = distance / 256.0;
    vec3 color = texture(gradientTexture, uv).rgb;
    FragColor = vec4(color, 1.0);
}