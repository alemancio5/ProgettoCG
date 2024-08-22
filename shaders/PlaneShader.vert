#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
    mat4 mMat;
} ubo;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNormal;

void main() {
    fragUV = inUV;
    fragPos = vec3(ubo.mMat * vec4(inPos, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(ubo.mMat))) * inNormal);
    gl_Position = ubo.mvpMat * vec4(inPos, 1.0);
}
