#version 330 core
in vec3 Position;
in vec3 vecColor;
in vec3 Normal;

out vec4 color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec3 cameraPos;
uniform samplerCube skybox;

struct PointLight {    
    vec3 position;
    vec3 intensity;
};
struct DirectionalLight {    
    vec3 direction;
    vec3 intensity;
};
struct Material{
    vec3 diffuseColor;
    vec3 specularColor;
    int glossiness;
};

#define NR_POINT_LIGHTS 10
#define NR_DIRECTIONAL_LIGHTS 10
#define NR_SHADOW_CASTS 10
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS];
uniform Material material;
uniform int pointLightCount;
uniform int directionalLightCount;
uniform int shadowCastersCount;
uniform vec3 shadowCasters[NR_SHADOW_CASTS];
uniform float shadowSize;

vec3 reflectDir(vec3 source, vec3 bouncePos, vec3 normal){
    vec3 posToLight = normalize(source - bouncePos);
    return (2 * normal * dot(posToLight, Normal) - posToLight);
}

void main()
{
    vec3 ambientLight = vec3(0.1, 0.1, 0.1);
    color = vec4(material.diffuseColor, 1.0f) * vec4((ambientLight), 1.0f);
    vec3 intensity = vec3(1,1,1);
    vec3 reflect = reflectDir(cameraPos, Position, Normal);

    //Phong lighting for point lights
    for (int i=0; i<pointLightCount; i++){
        float distanceToLight = length(pointLights[i].position - Position);
        vec3 posToLight = normalize(pointLights[i].position - Position);
        vec3 posToCamera = normalize(cameraPos - Position);
        float diffuse = clamp(dot(posToLight, Normal), 0, 1)/distanceToLight/distanceToLight;

        float specular = clamp(dot(reflect, posToLight), 0, 1);
        specular = pow(specular, material.glossiness)/distanceToLight/distanceToLight;
    
        color += vec4(material.diffuseColor, 1.0f) * (vec4(pointLights[i].intensity,1.0) * diffuse);
        color += vec4(material.specularColor, 1.0f) * (vec4(pointLights[i].intensity, 1.0) * specular);
    }

    //Phong lighting for directional lights
    for (int i=0; i<directionalLightCount; i++){
        vec3 posToLight = -1 * directionalLights[i].direction;
        vec3 posToCamera = normalize(cameraPos - Position);
        float diffuse = clamp(dot(posToLight, Normal), 0, 1);

        float specular = clamp(dot(reflect, posToLight), 0, 1);
        specular = pow(specular, material.glossiness);
    
        vec3 lColor = directionalLights[i].intensity;
        

        //Shadows/ambient occlusion
        float occlusion = 0;
        //float shadow_radious = 2;
        for (int i =0; i<shadowCastersCount; i++){
            float alpha = acos(clamp(dot(directionalLights[i].direction, normalize(vec3(Position-shadowCasters[i]))), 0, 1));
            occlusion += clamp(shadowSize - length(vec3(Position-shadowCasters[i]))*sin(alpha),0,1);
        }
        diffuse *= 1-occlusion;
        color += vec4(material.diffuseColor, 1.0f) * (vec4(lColor,1.0) * diffuse);
        color += vec4(material.specularColor, 1.0f) * (vec4(lColor, 1.0) * specular);
    }

    //Environment reflections
    vec3 envReflection = texture(skybox, reflect).rgb;
    //approximate fresnel reflection multiplier
    int invIOR = 2;
    envReflection *= 1/pow(clamp(dot(reflect, Normal),0, 1)+1, invIOR);
    color += vec4(envReflection*material.specularColor, 1.0);
    //color = 3* color;// / normalize(color);
    color = color / (color + vec4(0.1));
    
    
}
