#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D particleTexture;
uniform vec4 particleColor;

void main() {
    vec4 texColor = texture(particleTexture, TexCoords);
    FragColor = texColor * particleColor;
}