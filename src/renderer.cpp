#include "renderer.h"
#include <GLES3/gl3.h>
#include <emscripten/html5.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "mesh.h"
#include <vector>
#include "light.h"
#include "shadowFramebuffer.h"
#include "instancedMesh.h"

#ifdef __INTELLISENSE__
#define glBindVertexArray(x)
#define glGenVertexArrays(x, y)
#endif

GLuint program;
GLint uView;
GLint uProjection;
GLint uModel;

GLuint instancedProgram;
GLint uViewInstanced;
GLint uProjectionInstanced;
GLint uShadowMapInstanced;
GLint uLightSpaceMatrixInstanced;
GLint uLightPosInstanced;
GLint uLightColorInstanced;

GLuint instancedShadowProgram;
GLint uLightSpaceMatrixInstancedShadow;

GLuint shadowProgram;
GLint uLightSpaceMatrixShadow;
GLint uModelShadow;
Light light;
GLint uLightPos;
GLint uLightColor;

ShadowFramebuffer shadowFramebuffer;
GLint uLightSpaceMatrixMain;
GLint uShadowMap;

std::vector<Mesh> meshes;
std::vector<InstancedMesh> instancedMeshes;

std::string loadFile(const std::string &path)
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compileShader(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

void initRenderer()
{
    std::string vertStr = loadFile("shaders/basic.vert");
    std::string fragStr = loadFile("shaders/basic.frag");
    const char *vertSrc = vertStr.c_str();
    const char *fragSrc = fragStr.c_str();

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glUseProgram(program);

    uView = glGetUniformLocation(program, "uView");
    uProjection = glGetUniformLocation(program, "uProjection");
    uModel = glGetUniformLocation(program, "uModel");

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f);
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

    glEnable(GL_DEPTH_TEST);

    light.update();
    shadowFramebuffer.init(1024, 1024);

    uShadowMap = glGetUniformLocation(program, "uShadowMap");
    uLightSpaceMatrixMain = glGetUniformLocation(program, "uLightSpaceMatrix");
    uLightPos = glGetUniformLocation(program, "uLightPos");
    uLightColor = glGetUniformLocation(program, "uLightColor");
}

void initInstancedProgram()
{
    std::string vertStr = loadFile("shaders/instanced.vert");
    std::string fragStr = loadFile("shaders/basic.frag"); // reuse existing frag shader
    const char *vertSrc = vertStr.c_str();
    const char *fragSrc = fragStr.c_str();

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    instancedProgram = glCreateProgram();
    glAttachShader(instancedProgram, vert);
    glAttachShader(instancedProgram, frag);
    glLinkProgram(instancedProgram);
    glUseProgram(instancedProgram);

    uShadowMapInstanced = glGetUniformLocation(instancedProgram, "uShadowMap");
    uLightSpaceMatrixInstanced = glGetUniformLocation(instancedProgram, "uLightSpaceMatrix");
    uLightPosInstanced = glGetUniformLocation(instancedProgram, "uLightPos");
    uLightColorInstanced = glGetUniformLocation(instancedProgram, "uLightColor");

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    uViewInstanced = glGetUniformLocation(instancedProgram, "uView");
    glUniformMatrix4fv(uViewInstanced, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f);
    uProjectionInstanced = glGetUniformLocation(instancedProgram, "uProjection");
    glUniformMatrix4fv(uProjectionInstanced, 1, GL_FALSE, glm::value_ptr(projection));
}

void initInstancedShadowProgram()
{
    std::string vertStr = loadFile("shaders/instanced_shadow.vert");
    std::string fragStr = loadFile("shaders/shadow.frag"); // reuse shadow frag
    const char *vertSrc = vertStr.c_str();
    const char *fragSrc = fragStr.c_str();

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    instancedShadowProgram = glCreateProgram();
    glAttachShader(instancedShadowProgram, vert);
    glAttachShader(instancedShadowProgram, frag);
    glLinkProgram(instancedShadowProgram);

    uLightSpaceMatrixInstancedShadow = glGetUniformLocation(instancedShadowProgram, "uLightSpaceMatrix");
}

void initShadowProgram()
{
    std::string vertStr = loadFile("shaders/shadow.vert");
    std::string fragStr = loadFile("shaders/shadow.frag");
    const char *vertSrc = vertStr.c_str();
    const char *fragSrc = fragStr.c_str();

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    shadowProgram = glCreateProgram();
    glAttachShader(shadowProgram, vert);
    glAttachShader(shadowProgram, frag);
    glLinkProgram(shadowProgram);

    uLightSpaceMatrixShadow = glGetUniformLocation(shadowProgram, "uLightSpaceMatrix");
    uModelShadow = glGetUniformLocation(shadowProgram, "uModel");
}

void draw()
{

    // --- Pass 1: shadow pass ---
    shadowFramebuffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadowProgram);
    glUniformMatrix4fv(uLightSpaceMatrixShadow, 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));

    for (Mesh &mesh : meshes)
    {
        glUniformMatrix4fv(uModelShadow, 1, GL_FALSE, glm::value_ptr(mesh.transform.getMatrix()));
        mesh.draw();
    }
    glUseProgram(instancedShadowProgram);
    glUniformMatrix4fv(uLightSpaceMatrixInstancedShadow, 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));
    for (InstancedMesh &mesh : instancedMeshes)
    {
        mesh.draw();
    }
    shadowFramebuffer.unbind();

    // --- Pass 2: main pass ---
    glClearColor(0.3f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depthTexture);
    glUniform1i(uShadowMap, 0); // shadow map is texture unit 0
    glUniformMatrix4fv(uLightSpaceMatrixMain, 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));
    glUniform3fv(uLightPos, 1, glm::value_ptr(light.position));
    glUniform3fv(uLightColor, 1, glm::value_ptr(light.color));

    for (Mesh &mesh : meshes)
    {
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(mesh.transform.getMatrix()));
        mesh.draw();
    }

    // -  - - Instanced pass ---
    glUseProgram(instancedProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowFramebuffer.depthTexture);
    glUniform1i(uShadowMapInstanced, 0); // shadow map is texture unit 0
    glUniformMatrix4fv(uLightSpaceMatrixInstanced, 1, GL_FALSE, glm::value_ptr(light.lightSpaceMatrix));
    glUniform3fv(uLightPosInstanced, 1, glm::value_ptr(light.position));
    glUniform3fv(uLightColorInstanced, 1, glm::value_ptr(light.color));
    for (InstancedMesh &mesh : instancedMeshes)
    {
        mesh.draw();
    }
}