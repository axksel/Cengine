#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 2) in mat4 aInstanceMatrix;

uniform mat4 uLightSpaceMatrix;

void main()
{
    gl_Position = uLightSpaceMatrix * aInstanceMatrix * vec4(aPos, 1.0);
}