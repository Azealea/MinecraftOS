#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in ivec3 inBlockPos;
layout(location = 1) in int   inFaceId;
layout(location = 2) in int   inTextureId;

layout(location = 0) out vec3 fragTexCoord;

const ivec3 faceOffsets[6][4] = {
    {ivec3(1,0,0), ivec3(1,1,0), ivec3(1,1,1), ivec3(1,0,1)}, // +X
    {ivec3(0,0,1), ivec3(0,1,1), ivec3(0,1,0), ivec3(0,0,0)}, // -X
    {ivec3(0,1,1), ivec3(1,1,1), ivec3(1,1,0), ivec3(0,1,0)}, // +Y
    {ivec3(0,0,0), ivec3(1,0,0), ivec3(1,0,1), ivec3(0,0,1)}, // -Y
    {ivec3(1,0,1), ivec3(1,1,1), ivec3(0,1,1), ivec3(0,0,1)}, // +Z
    {ivec3(0,0,0), ivec3(0,1,0), ivec3(1,1,0), ivec3(1,0,0)}, // -Z
};

const vec2 faceUVs[6][4] = {
    {vec2(0,1), vec2(1,1), vec2(1,0), vec2(0,0)}, // +X
    {vec2(1,0), vec2(0,0), vec2(0,1), vec2(1,1)}, // -X
    {vec2(1,0), vec2(0,0), vec2(0,1), vec2(1,1)}, // +Y
    {vec2(0,1), vec2(1,1), vec2(1,0), vec2(0,0)}, // -Y
    {vec2(1,0), vec2(0,0), vec2(0,1), vec2(1,1)}, // +Z
    {vec2(0,1), vec2(1,1), vec2(1,0), vec2(0,0)}, // -Z
};

const int quadIndices[6] = {0, 1, 2, 2, 3, 0};

void main() {
    int v = quadIndices[gl_VertexIndex % 6];

    ivec3 worldPos = inBlockPos + faceOffsets[inFaceId][v];
    gl_Position  = ubo.proj * ubo.view * ubo.model * vec4(worldPos, 1.0);
    fragTexCoord = vec3(faceUVs[inFaceId][v], float(inTextureId));
}

