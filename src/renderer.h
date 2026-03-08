#pragma once
#include <GLES3/gl3.h>
#include <vector>
#include "mesh.h"
#include "light.h"
#include "shadowFramebuffer.h"
#include "instancedMesh.h"

extern ShadowFramebuffer shadowFramebuffer;

extern GLint uView; // extern camera gets updated from main.cpp atm
extern GLint uViewInstanced;

extern std::vector<Mesh> meshes;
extern std::vector<InstancedMesh> instancedMeshes;

extern GLuint program;
extern GLuint instancedProgram;

void initRenderer();
void initShadowProgram();
void initInstancedShadowProgram();
void initInstancedProgram();
void draw();