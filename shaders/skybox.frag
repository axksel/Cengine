#version 300 es
precision mediump float;

in vec3 vDir;
out vec4 fragColor;

layout(std140) uniform Light {
    mat4 uLightSpaceMatrix;
    vec3 uLightPos;
    vec3 uLightColor;
};

void main()
{
    vec3 dir = normalize(vDir);

    // gradient based on height
    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 skyColor = mix(vec3(0.6, 0.8, 1.0), vec3(0.1, 0.3, 0.8), t);

    // sun
    vec3 lightDir = normalize(uLightPos);
    float sun = pow(max(dot(dir, lightDir), 0.0), 64.0);
    skyColor += vec3(1.0, 0.9, 0.6) * sun;

    fragColor = vec4(skyColor, 1.0);
}