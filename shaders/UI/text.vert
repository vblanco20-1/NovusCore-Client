#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec2 fragTexCoord;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

void main() 
{
    fragTexCoord = inTexCoord;
    gl_Position = vec4((inPosition.xy * 2.0f) - 1.0f, inPosition.z, 1.0f);
}