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

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;
#define BR 2
struct Material {
    sampler2D tex;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
/*
struct PointLight {
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

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform sampler2D texture_diffuse1;
//uniform PointLight pointLight;

uniform bool light_enabled;
uniform vec3 lightPos;

uniform float constant;
uniform float linear;
uniform float quadratic;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

uniform samplerCube depthMap;
uniform float far_plane;
uniform bool shadows;

vec3 sampleOffsetDirections[20] = vec3[](
vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalc(vec3 fragPos){
    vec3 fragToLight = fragPos - lightPos;
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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight( vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color) {
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 ambient = ambient * color;
    vec3 diffuse = diffuse * diff * color;
    vec3 specular = specular * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, color);

    if (light_enabled) {
        result += CalcPointLight( norm, FragPos, viewDir, color);
    }
    FragColor = vec4(result, 1.0);
}
