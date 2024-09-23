#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNorm;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
    mat4 mMat;
    mat3 nMat;
} ubo;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNorm;

void main() {
    fragPos = vec3(ubo.mMat * vec4(inPos, 1.0)).xyz;
    fragNorm = mat3(ubo.nMat) * inNorm;
    fragUV = inUV;

    gl_Position = ubo.mvpMat * vec4(inPos, 1.0);
}
