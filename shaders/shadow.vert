#version 300 es
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
layout(std140) uniform Light {
    mat4 uLightSpaceMatrix;
    vec3 uLightPos;
    vec3 uLightColor;
};

void main() {
    gl_Position = uLightSpaceMatrix * uModel * vec4(aPos, 1.0);
}