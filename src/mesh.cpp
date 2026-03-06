#include "mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <map>

#ifdef __INTELLISENSE__
#define glBindVertexArray(x)
#define glGenVertexArrays(x, y)
#endif

void Mesh::load(const std::string &path)
{
    // Raw data read directly from the OBJ file
    std::vector<float> rawPositions; // all "v" lines
    std::vector<float> rawNormals;   // all "vn" lines

    // Final data we'll upload to the GPU
    std::vector<float> finalVertices;  // interleaved pos + normal - - GL_ARRAY_BUFFER
    std::vector<unsigned int> indices; // GL_ELEMENT_ARRAY_BUFFER

    std::map<std::pair<unsigned int, unsigned int>, unsigned int> seenVertices;

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
            rawPositions.push_back(x);
            rawPositions.push_back(y);
            rawPositions.push_back(z);
        }
        else if (prefix == "vn")
        {
            float x, y, z;
            ss >> x >> y >> z;
            rawNormals.push_back(x);
            rawNormals.push_back(y);
            rawNormals.push_back(z);
        }
        else if (prefix == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                std::string token;
                ss >> token;

                std::replace(token.begin(), token.end(), '/', ' ');
                std::stringstream ts(token);

                unsigned int vIdx, vtIdx, vnIdx;
                ts >> vIdx;
                ts >> vtIdx; // not doing UVs yet.
                ts >> vnIdx;

                std::pair<unsigned int, unsigned int> key;
                key.first = vIdx;
                key.second = vnIdx;

                if (seenVertices.count(key) > 0)
                {
                    // already built this vertex, reuse its index
                    indices.push_back(seenVertices[key]);
                }
                else
                {
                    // new vertex, build it
                    unsigned int newIndex = finalVertices.size() / 6;

                    finalVertices.push_back(rawPositions[(vIdx - 1) * 3 + 0]);
                    finalVertices.push_back(rawPositions[(vIdx - 1) * 3 + 1]);
                    finalVertices.push_back(rawPositions[(vIdx - 1) * 3 + 2]);
                    finalVertices.push_back(rawNormals[(vnIdx - 1) * 3 + 0]);
                    finalVertices.push_back(rawNormals[(vnIdx - 1) * 3 + 1]);
                    finalVertices.push_back(rawNormals[(vnIdx - 1) * 3 + 2]);

                    seenVertices[key] = newIndex;
                    indices.push_back(newIndex);
                }
            }
        }
    }
    indexCount = indices.size();

    // Upload to GPU
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, finalVertices.size() * sizeof(float), finalVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    printf("Loaded %s: %zu vertices, %zu indices\n", path.c_str(), finalVertices.size() / 6, indices.size());
}

void Mesh::draw()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}