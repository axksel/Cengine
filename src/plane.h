#pragma once
#include <GLES3/gl3.h>

struct Plane
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int indexCount;

    void init(int resolution, int size);
    void draw();
};