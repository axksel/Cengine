#include "playerControls.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "waves.h"

glm::vec3 playerPos = glm::vec3(0.0f);
float playerRotY = 0.0f;

static float speed = 0.0f;
static float maxSpeed = 0.08f;
static float acceleration = 0.002f;
static float deceleration = 0.04f; // drag
static float turnSpeed = 2.0f;     // degrees per frame

static float targetRollZ = 0.0f;
static float currentRollZ = 0.0f;
static float maxRoll = 15.0f;      // max tilt degrees
static float rollSmoothing = 0.1f; // how quickly it tilts

static SceneNode *playerNode;

void initPlayer(SceneNode *node)
{
    playerNode = node;
}

void updatePlayer(bool keys[256], float time)
{
    // --- Acceleration / deceleration ---
    if (keys[87])
        speed = glm::min(speed + acceleration, maxSpeed);
    else if (keys[83])
        speed = glm::max(speed - acceleration, -maxSpeed * 0.5f);
    else
        speed *= (1.0f - deceleration);

    // --- Turning ---
    float speedFactor = glm::abs(speed) / maxSpeed;
    float turnInput = 0.0f;
    if (keys[65])
    {
        playerRotY -= turnSpeed * speedFactor;
        turnInput = -1.0f;
    } // A
    if (keys[68])
    {
        playerRotY += turnSpeed * speedFactor;
        turnInput = 1.0f;
    } // D

    // --- Roll into turn ---
    targetRollZ = -turnInput * maxRoll * speedFactor;
    currentRollZ = glm::mix(currentRollZ, targetRollZ, rollSmoothing); // smooth lerp

    // --- Move forward ---
    float rad = glm::radians(playerRotY);
    glm::vec3 forward = glm::vec3(sin(rad), 0.0f, cos(rad));
    playerPos += forward * speed;
    playerPos.y = sampleWaveHeight(playerPos.x, playerPos.z, time);
    playerNode->transform.position = playerPos;

    glm::vec3 normal = sampleWaveNormal(playerPos.x, playerPos.z, time);

    // rotate normal into player local space
    glm::vec3 localNormal;
    localNormal.x = normal.x * cos(rad) - normal.z * sin(rad);
    localNormal.y = normal.y;
    localNormal.z = normal.x * sin(rad) + normal.z * cos(rad);

    float pitch = glm::degrees(atan2(localNormal.z, localNormal.y));
    float roll = glm::degrees(atan2(-localNormal.x, localNormal.y));

    playerNode->transform.rotation.x = pitch;
    playerNode->transform.rotation.y = playerRotY;          // add this here
    playerNode->transform.rotation.z = currentRollZ + roll; // keep this one
}