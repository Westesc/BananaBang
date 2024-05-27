#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;


uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform mat4 LSMatrix;

uniform sampler2D diffuseMap;
uniform sampler2D depthMap;

float shadowCal(vec4 FragPosLightSpace)
{
    vec3 projCoords = FragPosLightSpace.xyz/FragPosLightSpace.w;
    projCoords = projCoords *0.5 +0.5;
    float closestDepth = texture(depthMap,projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    //float shadow = currentDepth-bias > closestDepth ? 0.0 : 1.0;
    //return shadow;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{   
// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;  
        
    //shadow
    float shadow = shadowCal(LSMatrix * vec4(FragPos,1.0));
    vec3 result = (ambient + (1.0f -shadow) * (diffuse + specular))  ;
    FragColor = vec4(result.xyz, 1.0f)* texture(diffuseMap, TexCoords);
}