#pragma once
#include <GLES3/gl3.h>

struct Skybox
{
    GLuint vao;
    GLuint vbo;

    void init();
    void draw();
    ~Skybox();
};