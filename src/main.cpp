#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>

const char* vertSrc = R"(#version 300 es
    layout(location = 0) in vec2 aPos;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

const char* fragSrc = R"(#version 300 es
    precision mediump float;
    out vec4 fragColor;
    void main() {
        fragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
)";

GLuint vao;

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

void mainLoop() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main() {
    // Create WebGL context directly via html5 API
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2; // WebGL 2
    attrs.minorVersion = 0;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
    if (ctx <= 0) {
        printf("Failed to create WebGL context: %d\n", (int)ctx);
        return 1;
    }
    emscripten_webgl_make_context_current(ctx);

    // Shader program
    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glUseProgram(prog);

    // Triangle geometry
    float verts[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    emscripten_set_main_loop(mainLoop, 0, 1);
}