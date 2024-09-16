#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNorm;

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNorm;

void main() {
    fragUV = inUV * vec2(8, 8);
    fragPos = vec3(ubo.mMat * vec4(inPos, 1.0)).xyz;
    fragNorm = mat3(ubo.nMat) * inNorm;

    gl_Position = ubo.mvpMat * vec4(inPos, 1.0);
}
