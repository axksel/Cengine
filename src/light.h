#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light
{
    glm::vec3 position = glm::vec3(5.0f, 5.0f, 2.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 lightSpaceMatrix;

    void update(glm::vec3 playerPos)
    {
        // keep light centered around player
        glm::vec3 lightPos = playerPos + glm::vec3(-5.0f, 10.0f, -5.0f);
        position = playerPos + glm::vec3(-5.0f, 10.0f, -5.0f);

        lightSpaceMatrix = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 50.0f) * glm::lookAt(lightPos, playerPos, glm::vec3(0, 1, 0));
    }
};