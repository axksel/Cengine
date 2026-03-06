#version 300 es
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform mat4 uModel;

    out vec3 fragNormal;
    out vec3 fragPos;

    void main() {
        fragPos = vec3(uModel * vec4(aPos, 1.0));
        fragNormal = mat3(uModel) * aNormal;
        gl_Position = uProjection * uView * vec4(fragPos, 1.0);
    }