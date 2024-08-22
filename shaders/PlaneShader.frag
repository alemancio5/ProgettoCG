#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 fragNormal;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform AmbientLight {
    vec3 ambientColor;
};

layout(binding = 3) uniform DiffuseLight {
    vec3 lightPos;
    vec3 lightColor;
};

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragUV);
    vec3 ambient = ambientColor * texColor.rgb;
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * texColor.rgb;
    vec3 finalColor = ambient + diffuse;
    outColor = vec4(finalColor, texColor.a);
}
