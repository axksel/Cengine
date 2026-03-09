#version 300 es
precision mediump float;

in vec3 fragNormal;
in vec3 fragPos;

uniform sampler2D uShadowMap;
uniform vec3 uColor;

layout(std140) uniform Light {
    mat4 uLightSpaceMatrix;
    vec3 uLightPos;
    vec3 uLightColor;
};

out vec4 fragColor;

float shadow(vec3 fragPos) {
    vec4 fragPosLightSpace = uLightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(uShadowMap, 0));

    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
            shadow += currentDepth > texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r + 0.005 ? 1.0 : 0.0;

    shadow /= 9.0;
    return shadow;
}

void main() {

    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm     = normalize(fragNormal);
    vec3 lightDir = normalize(uLightPos - fragPos);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * uLightColor;

    float shadowValue = shadow(fragPos);
    
    vec3 objectColor = uColor;

    vec3 result = (ambient + (1.0 - shadowValue) * diffuse) * objectColor;
    fragColor = vec4(result, 1.0);
}
