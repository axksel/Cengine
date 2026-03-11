#version 300 es
precision mediump float;

in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragViewPos;

out vec4 fragColor;

layout(std140) uniform Light {
    mat4 uLightSpaceMatrix;
    vec3 uLightPos;
    vec3 uLightColor;
};

uniform sampler2D uShadowMap;

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

void main()
{
    // lighting
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * uLightColor;
    // wave height based color
    // fragPos.y will be between roughly -amplitude and +amplitude
    float height = clamp(fragPos.y * 4.0 + 0.5, 0.0, 1.0);

    vec3 deepColor     = vec3(0.0, 0.15, 0.4);
    vec3 shallowColor  = vec3(0.0, 0.5,  0.7);
    vec3 foamColor     = vec3(0.8, 0.9,  1.0);

    vec3 waterColor = mix(deepColor, shallowColor, height);
    waterColor = mix(waterColor, foamColor, pow(height, 4.0));

    //specular
    vec3 lightDir = normalize(uLightPos - fragPos);
    vec3 viewDir = normalize(fragViewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
    vec3 specular = spec * vec3(1.0, 1.0, 1.0) * 0.8;

     float shadowFactor = shadow(fragPos);

    vec3 result = (ambient * (1.0 - shadowFactor * 0.5) + specular * (1.0 - shadowFactor)) * waterColor;
    fragColor = vec4(result, 1.0);
}