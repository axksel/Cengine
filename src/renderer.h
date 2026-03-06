#pragma once
#include <GLES3/gl3.h>
#include <vector>
#include "mesh.h"

extern GLint uView;
extern GLint uModel;

extern std::vector<Mesh> meshes;

void initRenderer();
void draw();