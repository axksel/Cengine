#version 300 es
precision mediump float;

in vec3 fragNormal;
in vec3 fragPos;

out vec4 fragColor;

void main() {
    vec3 objectColor = vec3(1.0, 0.8, 0.2);
    vec3 lightColor  = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(2.0,5.0,2.0);

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm     = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * lightColor;

    // final color
    vec3 result = (ambient + diffuse) * objectColor;
    fragColor = vec4(result, 1.0);
}