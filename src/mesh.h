#pragma once
#include <GLES3/gl3.h>
#include <vector>
#include <string>
#include "transform.h"

struct Mesh
{
    GLuint vao; // Vertex Array Object
    GLuint vbo; // Vertex Buffer Object
    GLuint ebo; // Element Buffer Object
    int indexCount;
    Transform transform;

    void load(const std::string &path);
    void draw();
};