#pragma once
#include <glm/glm.hpp>
#include "sceneNode.h"

extern glm::vec3 playerPos;
extern float playerRotY;

extern int cargoCount;

void addCargo(int amount);
void initPlayer(SceneNode *node);
void updatePlayer(bool keys[256], float time);