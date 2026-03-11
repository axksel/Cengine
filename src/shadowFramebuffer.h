#pragma once
#include <GLES3/gl3.h>

struct ShadowFramebuffer
{
    GLuint fbo;
    GLuint depthTexture;
    int width;
    int height;

    void init(int width, int height);
    void bind();
    void unbind();
    ~ShadowFramebuffer();
};