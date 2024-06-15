#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos; 

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 M;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));

    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;

    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}