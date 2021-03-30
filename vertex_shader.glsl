#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 M;
uniform mat4 P;

out vec3 vColor;

void main()
{
    gl_Position =P*(M*vec4(pos,1.0));
    vColor = color;
}