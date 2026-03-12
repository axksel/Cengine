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

bool capsized = false;
static float capsizeAngle = 45.0f; // degrees before capsize

int cargoCount = 0;
static float maxCargo = 20.0f;

static SceneNode *playerNode;

static float cargoT()
{
    return glm::min((float)cargoCount / maxCargo, 1.0f); // 0 = empty, 1 = full
}

void addCargo(int amount)
{
    cargoCount = glm::min(cargoCount + amount, (int)maxCargo);
}

void initPlayer(SceneNode *node)
{
    playerNode = node;
}

void updatePlayer(bool keys[256], float time)
{

    if (capsized)
        return; // stop updating if capsized

    float t = cargoT();

    float currentMaxSpeed = glm::mix(maxSpeed, 0.03f, t);    // slows down
    float currentAccel = glm::mix(acceleration, 0.0008f, t); // more sluggish
    float currentTurnSpeed = glm::mix(turnSpeed, 0.8f, t);   // wider turns
    float currentMaxRoll = glm::mix(maxRoll, 30.0f, t);      // more dramatic tilt
    float waveInfluence = glm::mix(1.0f, 2.5f, t);           // more wave effect

    // --- Acceleration / deceleration ---
    if (keys[87])
        speed = glm::min(speed + currentAccel, currentMaxSpeed);
    else if (keys[83])
        speed = glm::max(speed - currentAccel, -currentMaxSpeed * 0.5f);
    else
        speed *= (1.0f - deceleration);

    // --- Turning ---
    float speedFactor = glm::abs(speed) / currentMaxSpeed;
    float turnInput = 0.0f;
    if (keys[65])
    {
        playerRotY -= currentTurnSpeed * speedFactor;
        turnInput = -1.0f;
    } // A
    if (keys[68])
    {
        playerRotY += currentTurnSpeed * speedFactor;
        turnInput = 1.0f;
    } // D

    if (keys[67])
        addCargo(1); // C key adds cargo -- DEBUG

    // --- Roll into turn ---
    targetRollZ = -turnInput * currentMaxRoll * speedFactor;
    currentRollZ = glm::mix(currentRollZ, targetRollZ, rollSmoothing); // smooth lerp

    // --- Move forward ---
    float rad = glm::radians(playerRotY);
    glm::vec3 forward = glm::vec3(sin(rad), 0.0f, cos(rad));
    playerPos += forward * speed;
    float currentDraft = glm::mix(0.0f, -0.2f, t); // sits lower as cargo increases
    playerPos.y = sampleWaveHeight(playerPos.x, playerPos.z, time) + currentDraft;
    playerNode->transform.position = playerPos;

    glm::vec3 normal = sampleWaveNormal(playerPos.x, playerPos.z, time);

    // rotate normal into player local space
    glm::vec3 localNormal;
    localNormal.x = normal.x * cos(rad) - normal.z * sin(rad);
    localNormal.y = normal.y;
    localNormal.z = normal.x * sin(rad) + normal.z * cos(rad);

    float pitch = glm::degrees(atan2(localNormal.z, localNormal.y));
    float roll = glm::degrees(atan2(-localNormal.x, localNormal.y));

    playerNode->transform.rotation.x = pitch * waveInfluence;               // pitch up/down based on wave normal
    playerNode->transform.rotation.y = playerRotY;                          // add this here
    playerNode->transform.rotation.z = currentRollZ + roll * waveInfluence; // keep this one

    float totalRoll = glm::abs(currentRollZ + roll * waveInfluence);
    float totalPitch = glm::abs(pitch * waveInfluence);

    if (totalRoll > capsizeAngle || totalPitch > capsizeAngle)
    {
        capsized = true;
        // Slam the ship on its side dramatically
        playerNode->transform.rotation.z = 90.0f;
        playerNode->transform.rotation.x = 0.0f;
        speed = 0.0f;
    }
}