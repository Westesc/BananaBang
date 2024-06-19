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
uniform sampler2D diffuseMap2;
uniform sampler2D depthMap;


float hash( vec2 p ) {
	float h = dot(p, vec2(12.9898, 4.1414));	
    return fract(sin(h) * 43758.5453);
}

vec2 grad( ivec2 z )  // replace this anything that returns a random vector
{
    // 2D to 1D  (feel free to replace by some other)
    int n = z.x+z.y*11111;

    // Hugo Elias hash (feel free to replace by another one)
    n = (n<<13)^n;
    n = (n*(n*n*15731+789221)+1376312589)>>16;

#if 0

    // simple random vectors
    return vec2(cos(float(n)),sin(float(n)));
    
#else

    // Perlin style vectors
    n &= 7;
    vec2 gr = vec2(n&1,n>>1)*2.0-1.0;
    return ( n>=6 ) ? vec2(0.0,gr.x) : 
           ( n>=4 ) ? vec2(gr.x,0.0) :
                              gr;
#endif                              
}

float noise( in vec2 p ) {
    ivec2 i = ivec2(floor( p ));
     vec2 f =       fract( p );
	
	vec2 u = f*f*(3.0-2.0*f); // feel free to replace by a quintic smoothstep instead

    return mix( mix( 
    dot( grad( i+ivec2(0,0) ), f-vec2(0.0,0.0) ), dot( grad( i+ivec2(1,0) ), f-vec2(1.0,0.0) ), u.x),
                mix( dot( grad( i+ivec2(0,1) ), f-vec2(0.0,1.0) ), 
                     dot( grad( i+ivec2(1,1) ), f-vec2(1.0,1.0) ), u.x), u.y);
}

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
    vec3 lightDir = normalize(vec3(1.0f,0.8f,0.f));
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
    vec3 result = (ambient + (1.0f -shadow) * (diffuse))  ;

    float noiseValue= noise(vec2(FragPos.xz)/vec2(5.f));
    //vec3 colorGrass = vec3(0.0f,0.5f,0.f);
    //vec3 colorGround = vec3(0.1225f,0.3059f,0.0089f)*(1-noiseValue);
    vec3 colorGround = texture(diffuseMap2,TexCoords).rgb*(1-noiseValue);
    vec3 colorGrass = texture(diffuseMap,TexCoords).rgb*(noiseValue);
    //FragColor =  vec4(result.xyz, 1.0f)*vec4(mix(colorGround,colorGrass,noiseValue),1.0f);
    FragColor =  vec4(result.xyz, 1.0f)*vec4(colorGrass+ colorGround,1.0f);
}