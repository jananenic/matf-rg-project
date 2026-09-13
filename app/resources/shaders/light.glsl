//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    FragPos = vec3(model * vec4(aPos,1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos,1.0);
}

//#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
#define BR 2
struct Material {
    sampler2D tex;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
/*
struct PointLight {
    bool light_enabled;
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
*/


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 pointPos;
uniform float constant;
uniform float linear;
uniform float quadratic;

uniform vec3 ambientP;
uniform vec3 diffuseP;
uniform vec3 specularP;


uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform sampler2D texture_diffuse1;
//uniform PointLight pointLights[BR];

uniform bool light_enabled;

uniform samplerCube depthMap;
uniform vec3 lightPos;
uniform float far_plane;
uniform bool shadows;

vec3 sampleOffsetDirections[20] = vec3[](
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);

float ShadowCalc(vec3 fragPos){
    vec3 fragToLight = fragPos - pointPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;  //0.005
    int samples = 20;

    float viewDist = length(viewPos - fragPos);
    float diskRadius = (0.1 + (viewDist / far_plane)) / 25.0;

    for(int i = 0; i < samples; ++i){
        float closestDept = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDept *= far_plane;
        if(currentDepth - bias > closestDept)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), 32.0);

    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec ;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color){
    vec3 lightDir = normalize(pointPos - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), 32.0);

    float distance = length(pointPos - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 ambient = ambientP * color;
    vec3 diffuse = diffuseP * diff *color;
    vec3 specular = specularP * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //float shadow = 0;
    float shadow = shadows ? ShadowCalc(fragPos) : 0.0;
    float shadowFactor = 1.0 - shadow;
    return (ambient + (1-shadowFactor)* (diffuse + specular));
}

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-FragPos);

    vec3 result = CalcDirLight(dirLight,norm, viewDir, color);

    if(light_enabled){
        for(int i = 0; i < BR-1; i++) {
            result += CalcPointLight(norm, FragPos, viewDir, color);
        }
    }

    FragColor = vec4(result, 1.0);
}
