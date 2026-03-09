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
    // transform fragment position into light space
    vec4 fragPosLightSpace = uLightSpaceMatrix * vec4(fragPos, 1.0);
    
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to 0-1 range
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0; // check if its outside the light's range
    
    // sample shadow map
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    // check if in shadow
    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.0;
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
