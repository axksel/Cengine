#include "meshImporter.h"
#include "mesh.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

MeshData importOBJ(const std::string &path)
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
        return {};
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
    return MeshData{finalVertices, indices};
}