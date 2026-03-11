#pragma once
#include <GLES3/gl3.h>

struct ColorFramebuffer
{
    GLuint fbo;
    GLuint depthTexture;
    GLuint colorTexture;

    int width;
    int height;

    void init(int width, int height);
    void bind();
    void unbind();
    ~ColorFramebuffer();
};