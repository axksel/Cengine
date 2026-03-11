#pragma once
#include <GLES3/gl3.h>

struct FullScreenQuad
{
    GLuint vao;
    GLuint vbo;

    void init();
    void draw();
    ~FullScreenQuad();
};