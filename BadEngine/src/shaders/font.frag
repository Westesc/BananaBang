#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform vec3 color;
uniform bool isText;

void main()
{    
    if(isText){
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(diffuseMap, TexCoords).r);
        FragColor = vec4(color, 1.0) * sampled;
    }
    else{
        FragColor = texture(diffuseMap,TexCoords);
    }
}