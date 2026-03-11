// waves.cpp
#include "waves.h"
#include <cmath>
#include <glm/glm.hpp>

struct GerstnerWave
{
    float dirX, dirZ;
    float amplitude;
    float wavelength;
    float speed;
};

static float gerstnerWave(float x, float z, GerstnerWave w, float time)
{
    float len = sqrt(w.dirX * w.dirX + w.dirZ * w.dirZ);
    float dx = w.dirX / len;
    float dz = w.dirZ / len;
    float freq = 2.0f * 3.14159f / w.wavelength;
    float phi = w.speed * freq;
    float f = freq * (dx * x + dz * z) + phi * time;
    return w.amplitude * sin(f);
}

float sampleWaveHeight(float x, float z, float time)
{
    float y = 0.0f;
    y += gerstnerWave(x, z, {1.0f, 0.2f, 0.1f, 4.0f, 1.3f}, time);
    y += gerstnerWave(x, z, {-0.8f, 0.6f, 0.05f, 3.0f, 1.5f}, time);
    y += gerstnerWave(x, z, {-0.8f, -0.7f, 0.01f, 2.0f, 1.1f}, time);
    return y;
}

glm::vec3 sampleWaveNormal(float x, float z, float time)
{
    float eps = 0.1f;
    float y = sampleWaveHeight(x, z, time);
    float yx = sampleWaveHeight(x + eps, z, time);
    float yz = sampleWaveHeight(x, z + eps, time);

    glm::vec3 tangentX = glm::normalize(glm::vec3(eps, yx - y, 0.0f));
    glm::vec3 tangentZ = glm::normalize(glm::vec3(0.0f, yz - y, eps));
    return glm::normalize(glm::cross(tangentZ, tangentX));
}