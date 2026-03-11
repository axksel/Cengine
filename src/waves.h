// waves.h
#pragma once
#include <glm/glm.hpp>

float sampleWaveHeight(float x, float z, float time);
glm::vec3 sampleWaveNormal(float x, float z, float time);