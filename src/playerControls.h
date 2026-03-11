#pragma once
#include <glm/glm.hpp>
#include "sceneNode.h"

extern glm::vec3 playerPos;
extern float playerRotY;

void initPlayer(SceneNode *node);
void updatePlayer(bool keys[256], float time);