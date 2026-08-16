#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vNormal;
out vec3 vWorldPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vWorldPos = (model * vec4(aPos, 1.0)).xyz;
    vNormal = mat3(transpose(inverse(model))) * aNormal;
}