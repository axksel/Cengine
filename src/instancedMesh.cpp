#include "instancedMesh.h"
#include "mesh.h"

#ifdef __INTELLISENSE__
#define glBindVertexArray(x)
#define glGenVertexArrays(x, y)
#define glDrawElementsInstanced(a, b, c, d, e)
#define glVertexAttribDivisor(a, b)
#endif

void InstancedMesh::load(const std::string &path)
{
    mesh.load(path); // handles all the geometry upload

    // reopen the same VAO to attach instance VBO
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);

    for (int i = 0; i < 4; i++)
    {
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(2 + i);
        glVertexAttribDivisor(2 + i, 1);
    }

    glBindVertexArray(0);
}
void InstancedMesh::draw()
{
    glBindVertexArray(mesh.vao);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr, instances.size());
}

void InstancedMesh::addInstance(const glm::mat4 &matrix)
{
    instances.push_back(matrix);
}

void InstancedMesh::uploadInstances()
{
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(glm::mat4), instances.data(), GL_STATIC_DRAW);
}