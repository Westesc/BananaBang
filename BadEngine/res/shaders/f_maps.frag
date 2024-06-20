#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap; // Add this line

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{           
    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // This normal is in tangent space
   
    // Get diffuse color
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    // Ambient
    vec3 ambient = 0.1 * color;
    // Diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  

    // Sample from specular map to get specular intensity
    float specIntensity = texture(specularMap, fs_in.TexCoords).r;
    vec3 specularColor = vec3(1.0); // Adjust this to control specular color
    vec3 specular = specIntensity * specularColor; // Use specular intensity

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}