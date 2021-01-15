#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec3 vecColor;
out vec3 Normal;
out vec3 Position;
  
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //gl_Position = projection * view * transform * vec4(position, 1.0f);
    gl_Position = projection * view * transform * vec4(position, 1.0f);

    vecColor = color;
    Normal = normalize(mat3(transform) * normal);
    //Normal = normal;
    Position = vec3(transform * vec4(position, 1.0f));
} 