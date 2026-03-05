#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include "renderer.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Global key state
bool keys[256] = {};

// Keyboard callbacks
EM_BOOL keyDown(int type, const EmscriptenKeyboardEvent *e, void *userData)
{
    keys[e->keyCode] = true;
    return EM_TRUE;
}

EM_BOOL keyUp(int type, const EmscriptenKeyboardEvent *e, void *userData)
{
    keys[e->keyCode] = false;
    return EM_TRUE;
}

void mainLoop()
{
    float speed = 0.05f;
    if (keys[87])
        cameraPos += speed * cameraFront; // W
    if (keys[83])
        cameraPos -= speed * cameraFront; // S
    if (keys[65])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed; // A
    if (keys[68])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed; // D

    // Recalculate view matrix
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

    draw();
}

int main()
{
    // Create WebGL context directly via html5 API
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2; // WebGL 2
    attrs.minorVersion = 0;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
    if (ctx <= 0)
    {
        printf("Failed to create WebGL context: %d\n", (int)ctx);
        return 1;
    }
    emscripten_webgl_make_context_current(ctx);

    initRenderer();

    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, keyDown);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, keyUp);

    emscripten_set_main_loop(mainLoop, 0, 1);
}