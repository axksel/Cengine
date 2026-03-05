#include "mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>

#ifdef __INTELLISENSE__
#define glBindVertexArray(x)
#define glGenVertexArrays(x, y)
#endif

void Mesh::load(const std::string &path)
{
    std::vector<float> positions;
    std::vector<unsigned int> indices;

    std::ifstream file(path);
    if (!file.is_open())
    {
        printf("Failed to open file: %s\n", path.c_str());
        return;
    }
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            ss >> x >> y >> z;
            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        }
        else if (prefix == "f")
        {
            unsigned int a, b, c;
            ss >> a >> b >> c;
            indices.push_back(a - 1); // OBJ indices start at 1
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }

    indexCount = indices.size();

    // Upload to GPU
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    printf("Loaded %s: %zu vertices, %zu indices\n", path.c_str(), positions.size() / 3, indices.size());
}

void Mesh::draw()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}