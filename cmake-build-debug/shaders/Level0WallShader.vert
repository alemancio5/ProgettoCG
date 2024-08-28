#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

const int LEVEL_0_WALL=10;
layout(binding = 0) uniform UniformBufferObject {
    mat4 mvpMat[LEVEL_0_WALL];
    mat4 mMat[LEVEL_0_WALL];
    mat4 nMat[LEVEL_0_WALL];
} ubo;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNormal;

void main() {
    int i = gl_InstanceIndex;

    gl_Position = ubo.mvpMat[i] * vec4(inPos, 1.0);

    fragPos = vec3(ubo.mMat[i] * vec4(inPos, 1.0));
    fragNormal = vec3(ubo.nMat[i] * vec4(inNormal, 0.0));
    fragUV = inUV * vec2(6, 150);
}
