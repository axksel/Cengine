#version 300 es
layout(location = 0) in vec3 aPos;

layout(std140) uniform Camera {
    mat4 uView;
    mat4 uProjection;
};

out vec3 vDir;

void main()
{
    vDir = aPos;

    // remove translation from view matrix so skybox stays centered on camera
    mat4 viewNoTranslation = mat4(mat3(uView));

    gl_Position = uProjection * viewNoTranslation * vec4(aPos, 1.0);
}