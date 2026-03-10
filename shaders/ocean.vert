#version 300 es
layout(location = 0) in vec3 aPos;

layout(std140) uniform Camera {
    mat4 uView;
    mat4 uProjection;
};

uniform float uTime;

out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragViewPos;

vec3 gerstnerWave(vec3 pos, vec2 dir, float amplitude, float wavelength, float speed)
{
    float w = 2.0 * 3.14159 / wavelength;
    float phi = speed * w;
    float f = w * dot(dir, pos.xz) + phi * uTime;

    vec3 displacement;
    displacement.x = dir.x * amplitude * cos(f);
    displacement.y = amplitude * sin(f);
    displacement.z = dir.y * amplitude * cos(f);
    return displacement;
}

void main()
{
    vec3 pos = aPos;

    // large swells
    pos += gerstnerWave(aPos, normalize(vec2(1.0,  0.2)), 0.1, 4.0, 1.3);
    pos += gerstnerWave(aPos, normalize(vec2(-0.8, 0.6)),  0.05, 3.0, 1.5);
    pos += gerstnerWave(aPos, normalize(vec2(-0.8, -0.7)),  0.01, 2.0, 1.1);

    // finite difference for normal
    float eps = 0.01;
    vec3 posX = aPos + vec3(eps, 0.0, 0.0);
    vec3 posZ = aPos + vec3(0.0, 0.0, eps);

    posX += gerstnerWave(posX, normalize(vec2(1.0,  0.2)), 0.1, 4.0, 1.3);
    posX += gerstnerWave(posX, normalize(vec2(-0.8, 0.6)),  0.05, 3.0, 1.5);
    posX += gerstnerWave(posX, normalize(vec2(-0.8, -0.7)),  0.01, 2.0, 1.1);

    posZ += gerstnerWave(posZ, normalize(vec2(1.0,  0.2)), 0.1, 4.0, 1.3);
    posZ += gerstnerWave(posZ, normalize(vec2(-0.8, 0.6)),  0.05, 3.0, 1.5);
    posZ += gerstnerWave(posZ, normalize(vec2(-0.8, -0.7)),  0.01, 2.0, 1.1);

    fragNormal = normalize(cross(posZ - pos, posX - pos));
    fragPos = pos;

    fragViewPos = (uView * vec4(pos, 1.0)).xyz;

    gl_Position = uProjection * uView * vec4(pos, 1.0);
}