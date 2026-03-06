#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include "renderer.h"
#include "mesh.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

// Global key state
bool keys[256] = {};

// Keyboard callbacks
bool keyDown(int type, const EmscriptenKeyboardEvent *e, void *userData)
{
    keys[e->keyCode] = true;
    return true;
}

bool keyUp(int type, const EmscriptenKeyboardEvent *e, void *userData)
{
    keys[e->keyCode] = false;
    return true;
}

bool mouseMove(int type, const EmscriptenMouseEvent *e, void *userData)
{
    float sensitivity = 0.1f;
    yaw += e->movementX * sensitivity;
    pitch -= e->movementY * sensitivity;

    // Clamp pitch so camera doesn't flip
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Recalculate camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    return true;
}

bool mouseClick(int type, const EmscriptenMouseEvent *e, void *userData)
{
    emscripten_request_pointerlock("#canvas", EM_TRUE);
    return true;
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
    if (keys[81])
        cameraPos -= speed * cameraUp; // Q
    if (keys[69])
        cameraPos += speed * cameraUp; // E

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
    initShadowProgram();

    Mesh plane;
    plane.load("models/plane.obj");
    plane.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    meshes.push_back(plane);

    Mesh head;
    head.load("models/animal-horse.obj");
    head.transform.position = glm::vec3(1.0f, 0.0f, 0.0f);
    meshes.push_back(head);

    Mesh head2;
    head2.load("models/sphere.obj");
    head2.transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);
    meshes.push_back(head2);

    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, keyDown);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, keyUp);

    emscripten_set_mousemove_callback("#canvas", nullptr, EM_TRUE, mouseMove);
    emscripten_set_click_callback("#canvas", nullptr, EM_TRUE, mouseClick);

    emscripten_set_main_loop(mainLoop, 0, 1);
}