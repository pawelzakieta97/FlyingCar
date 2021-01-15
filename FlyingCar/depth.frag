#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec3 cameraPos;

struct PointLight {    
    vec3 position;
    vec3 intensity;
};
struct Material{
    vec3 diffuseColor;
    vec3 specularColor;
    int glossiness;
};

#define NR_POINT_LIGHTS 20
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform int pointLightCount;

vec3 reflectDir(vec3 source, vec3 bouncePos, vec3 normal){
    vec3 posToLight = normalize(source - bouncePos);
    return (2 * normal * dot(posToLight, Normal) - posToLight);
}

void main()
{
    vec3 ambientLight = vec3(0.1, 0.1, 0.1);
    color = vec4(material.diffuseColor, 1.0f) * vec4((ambientLight), 1.0f);
    vec3 intensity = vec3(1,1,1);
    for (int i=0; i<pointLightCount; i++){
        float distanceToLight = length(pointLights[i].position - Position);
        vec3 posToLight = normalize(pointLights[i].position - Position);
        vec3 posToCamera = normalize(cameraPos - Position);
        float diffuse = clamp(dot(posToLight, Normal), 0, 1)/distanceToLight/distanceToLight;

        vec3 reflect = reflectDir(pointLights[i].position, Position, Normal);
        //float glossiness = 9;
        float specular = clamp(dot(reflect, posToCamera), 0, 1);
        specular = pow(specular, material.glossiness)/distanceToLight/distanceToLight;
        vec3 specularColor = vec3(2.0,2.0,2.0);
    
        color += vec4(material.diffuseColor, 1.0f) * (vec4(pointLights[i].intensity,1.0) * diffuse);
        color += vec4(material.specularColor, 1.0f) * (vec4(pointLights[i].intensity, 1.0) * specular);
    }
    //color = 3* color;// / normalize(color);
    color = color / (color + vec4(1));
    
}
