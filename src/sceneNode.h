#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "mesh.h"
#include "transform.h"

struct SceneNode
{
    Transform transform;
    std::vector<Mesh *> meshes;
    std::vector<SceneNode *> children;
    SceneNode *parent = nullptr;

    glm::mat4 getWorldMatrix();
    void addChild(SceneNode *child);
    void draw(glm::mat4 parentMatrix, GLint uModel, GLint uColor);
};
