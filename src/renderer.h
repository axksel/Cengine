#pragma once
#include <GLES3/gl3.h>
#include <vector>
#include "mesh.h"
#include "light.h"
#include "shadowFramebuffer.h"

extern ShadowFramebuffer shadowFramebuffer;

extern GLint uView;
extern GLint uModel;
extern Light light;

extern std::vector<Mesh> meshes;

void initRenderer();
void initShadowProgram();
void draw();