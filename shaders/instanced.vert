#version 300 es
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in mat4 aInstanceMatrix;
    
layout(std140) uniform Camera {
    mat4 uView;
    mat4 uProjection;
};
    uniform mat4 uModel;

    out vec3 fragNormal;
    out vec3 fragPos;

    void main() {
        vec4 worldPos = aInstanceMatrix * vec4(aPos, 1.0);
        fragPos = worldPos.xyz;
        fragNormal = mat3(aInstanceMatrix) * aNormal;
        gl_Position = uProjection * uView * worldPos;
    }