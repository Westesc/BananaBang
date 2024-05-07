#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightPos;
uniform sampler2D gradientTexture;

void main()
{
    float distance = length(lightPos - FragPos);
    float uv = distance / 256.0;
    if(uv > 1.0) { uv = 1.0; }
    vec3 color = texture(gradientTexture, vec2(uv,0.0)).rgb;
    FragColor = vec4(color, 1.0);
}