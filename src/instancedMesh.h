#pragma once
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "mesh.h"

struct InstancedMesh
{
    Mesh mesh;
    GLuint instanceVbo;
    std::vector<glm::mat4> instances;

    void load(const std::string &path);
    void addInstance(const glm::mat4 &matrix);
    void uploadInstances();
    void draw();
};