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
#define GL_UNIFORM_BUFFER 0
#define glBindBufferBase(x, y, z)
#define glUniformBlockBinding(x, y, z)
#define glGetUniformBlockIndex(x, y) 0
#endif

GLuint cameraUBO;
GLuint lightUBO;

GLuint program;
GLint uModel;

GLuint instancedProgram;
GLint uShadowMapInstanced;

GLuint instancedShadowProgram;
GLint uLightSpaceMatrixInstancedShadow;

GLuint shadowProgram;
GLint uLightSpaceMatrixShadow;
GLint uModelShadow;
Light light;

ShadowFramebuffer shadowFramebuffer;
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

    uModel = glGetUniformLocation(program, "uModel");

    // Camera UBO setup
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO); // bind to point 0

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));                       // offset 0
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)); // offset 64

    GLuint camIndex = glGetUniformBlockIndex(program, "Camera");
    glUniformBlockBinding(program, camIndex, 0);

    // Light and shadow setup and UBO
    light.update();
    shadowFramebuffer.init(1024, 1024);
    uShadowMap = glGetUniformLocation(program, "uShadowMap");

    glGenBuffers(1, &lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + 2 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightUBO); // binding point 1

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(light.lightSpaceMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(light.position));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(light.color));

    GLuint lightIndex = glGetUniformBlockIndex(program, "Light");
    glUniformBlockBinding(program, lightIndex, 1);

    glEnable(GL_DEPTH_TEST);
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
    GLuint lightIndex = glGetUniformBlockIndex(instancedProgram, "Light");
    glUniformBlockBinding(instancedProgram, lightIndex, 1);

    GLuint camIndex = glGetUniformBlockIndex(instancedProgram, "Camera");
    glUniformBlockBinding(instancedProgram, camIndex, 0);
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
    for (InstancedMesh &mesh : instancedMeshes)
    {
        mesh.draw();
    }
}