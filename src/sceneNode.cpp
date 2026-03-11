#include "sceneNode.h"
#include <algorithm>

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

void SceneNode::destroy(std::vector<SceneNode *> &sceneNodes)
{
    // remove from parent's children
    if (parent)
    {
        auto &siblings = parent->children;
        siblings.erase(
            std::remove(siblings.begin(), siblings.end(), this),
            siblings.end());
    }
    else
    {
        // it's a root node, remove from sceneNodes
        sceneNodes.erase(
            std::remove(sceneNodes.begin(), sceneNodes.end(), this),
            sceneNodes.end());
    }

    // recursively destroy children
    for (SceneNode *child : children)
        child->destroy(sceneNodes);

    delete this;
}