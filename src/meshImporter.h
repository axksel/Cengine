#pragma once
#include <vector>
#include <string>

struct MeshData
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

MeshData importOBJ(const std::string &path);