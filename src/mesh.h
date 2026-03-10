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
    glm::vec3 color = glm::vec3(1.0f);

    void load(const std::string &path);
    void draw(glm::mat4 modelMatrix, GLint uModel, GLint uColor);
};