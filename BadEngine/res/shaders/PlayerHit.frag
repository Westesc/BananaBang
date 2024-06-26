#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float timeSinceHit;

void main() {
    vec4 color = texture(screenTexture, TexCoords);
    float intensity = max(0.0, 1.0 - timeSinceHit / 1.0);
    vec3 reddish = vec3(1.0, 0.0, 0.0);
    FragColor = mix(color, vec4(reddish, 1.0), intensity * 0.5);
}