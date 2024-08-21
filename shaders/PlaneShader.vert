#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvpMat;
} ubo;

layout(location = 0) out vec2 fragUV;

void main() {
    fragUV = inUV;
    gl_Position = ubo.mvpMat * vec4(inPos, 1.0);
}
