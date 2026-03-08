#version 300 es
precision mediump float;

in vec3 fragNormal;
in vec3 fragPos;

uniform sampler2D uShadowMap;
uniform mat4 uLightSpaceMatrix;
uniform vec3 uLightPos;
uniform vec3 uLightColor;

out vec4 fragColor;


float hash(vec3 p) {
    p = fract(p * vec3(443.897, 441.423, 437.195));
    p += dot(p, p.yxz + 19.19);
    return fract((p.x + p.y) * p.z);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);

    // smooth interpolation
    vec3 u = f * f * (3.0 - 2.0 * f);

    float a = hash(i + vec3(0,0,0));
    float b = hash(i + vec3(1,0,0));
    float c = hash(i + vec3(0,1,0));
    float d = hash(i + vec3(1,1,0));
    float e = hash(i + vec3(0,0,1));
    float f2 = hash(i + vec3(1,0,1));
    float g = hash(i + vec3(0,1,1));
    float h = hash(i + vec3(1,1,1));

    return mix(mix(mix(a,b,u.x), mix(c,d,u.x), u.y),
               mix(mix(e,f2,u.x), mix(g,h,u.x), u.y), u.z);
}

float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}



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
    
    float n = fbm(fragPos * 5.0);
    vec3 objectColor = vec3(n, n * 0.5, 0.2);

    vec3 result = (ambient + (1.0 - shadowValue) * diffuse) * objectColor;
    fragColor = vec4(result, 1.0);
}
