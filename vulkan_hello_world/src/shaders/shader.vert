#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in ivec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in int texLayer;

layout(location = 0) out vec3 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragTexCoord = vec3(inTexCoord, texLayer);
}

