#include "skybox.h"

#ifdef __INTELLISENSE__
#define glBindVertexArray(x)
#define glGenVertexArrays(x, y)
#endif

void Skybox::init()
{

    float vertices[] = {
        -1, -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, -1, // back
        -1, -1, 1, 1, -1, 1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, -1, 1,       // front
        -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, 1, 1, // left
        1, 1, 1, 1, 1, -1, 1, -1, -1, 1, -1, -1, 1, -1, 1, 1, 1, 1,       // right
        -1, -1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, 1, -1, -1, -1, // bottom
        -1, 1, -1, 1, 1, -1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, 1, -1        // top
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Skybox::draw()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}