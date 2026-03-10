#include "sceneNode.h"

// maybe needed for later.
glm::mat4 SceneNode::getWorldMatrix()
{
    glm::mat4 worldMatrix = transform.getMatrix();
    SceneNode *parentRef = parent;
    while (parentRef)
    {
        worldMatrix = parentRef->transform.getMatrix() * worldMatrix;
        parentRef = parentRef->parent;
    }
    return worldMatrix;
}

void SceneNode::addChild(SceneNode *child)
{
    children.push_back(child);
    child->parent = this;
}

void SceneNode::draw(glm::mat4 parentMatrix, GLint uModel, GLint uColor)
{
    glm::mat4 worldMatrix = parentMatrix * transform.getMatrix();
    for (const auto &mesh : meshes)
    {
        mesh->draw(worldMatrix, uModel, uColor);
    }
    for (SceneNode *child : children)
    {
        child->draw(worldMatrix, uModel, uColor);
    }
}