#pragma once
#include <GLES3/gl3.h>
#include <vector>
#include <string>

struct Mesh
{
    GLuint vao; // Vertex Array Object
    GLuint vbo; // Vertex Buffer Object
    GLuint ebo; // Element Buffer Object
    int indexCount;

    void load(const std::string &path);
    void draw();
};