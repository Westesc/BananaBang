#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightPos;

void main()
{
    float distance = length(lightPos - FragPos);

    vec3 color1 = vec3(1.0, 0.0, 0.0);
    vec3 color2 = vec3(0.0, 1.0, 0.0);
    vec3 color3 = vec3(0.0, 0.0, 1.0);

    float distance1 = 5.0;
    float distance2 = 10.0;


    vec3 finalColor;
    if (distance <= distance1) {
        finalColor = color1;
    } else if (distance <= distance2) {
        float t = (distance - distance1) / (distance2 - distance1);
        finalColor = mix(color1, color2, t);
    } else {
        float t = (distance - distance2) / (distance2 * 2.0);
        finalColor = mix(color2, color3, t);
    }

    FragColor = vec4(finalColor, 1.0);
}