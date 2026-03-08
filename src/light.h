#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light
{
    glm::vec3 position = glm::vec3(5.0f, 5.0f, 2.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 lightSpaceMatrix;

    void update()
    {
        glm::mat4 lightView = glm::lookAt(
            position,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lightProjection = glm::ortho(
            -5.0f, 5.0f,
            -5.0f, 5.0f,
            0.1f, 25.0f);

        lightSpaceMatrix = lightProjection * lightView;
    }
};