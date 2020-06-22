#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension VK_EXT_descriptor_indexing : enable
#extension GL_EXT_scalar_block_layout : enable

// Textures
layout(set = 3, binding = 0) uniform sampler alphaSampler;
layout(set = 4, binding = 0) uniform sampler colorSampler;
layout(set = 5, binding = 0) uniform texture2D terrainColorTextures[4096];
layout(set = 6, binding = 0) uniform texture2DArray terrainAlphaTextures[196];

struct ChunkData
{
	uvec4 diffuseIDs;
};
layout(set = 7, binding = 0, std430) uniform ChunkDataBuffer
{
    ChunkData chunkDatas[256];
};

// From vertex shader
layout(location = 0) flat in uint fragInstanceID;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
	// Our UVs currently go between 0 and 8, with wrapping. This is correct for terrain color textures
	vec2 uv = fragTexCoord.xy; // [0.0 .. 8.0]

	// However the alpha needs to be between 0 and 1, so lets convert it
	vec3 alphaUV = vec3(uv / 8.0, fragInstanceID); // [0.0 .. 1.0]

	// We have 4 uints per chunk for our diffuseIDs, this gives us a size and alignment of 16 bytes which is exactly what GPUs want
	// However, we need a fifth uint for alphaID, so we decided to pack it into the LAST diffuseID, which gets split into two uint16s
	// This is what it looks like
	// [1111] diffuseIDs[0]
	// [2222] diffuseIDs[1]
	// [3333] diffuseIDs[2]
	// [AA44] diffuseIDs[3] Alpha is read from the most significant bits, the fourth diffuseID read from the least 
	uint diffuse0ID = chunkDatas[fragInstanceID].diffuseIDs[0];
	uint diffuse1ID = chunkDatas[fragInstanceID].diffuseIDs[1];
	uint diffuse2ID = chunkDatas[fragInstanceID].diffuseIDs[2];
	uint diffuse3ID = chunkDatas[fragInstanceID].diffuseIDs[3] & 0xFFFF;
	uint alphaID	= chunkDatas[fragInstanceID].diffuseIDs[3] >> 16;
	
	vec3 alpha = texture(sampler2DArray(terrainAlphaTextures[alphaID], alphaSampler), alphaUV).rgb;

	vec4 diffuse0 = texture(sampler2D(terrainColorTextures[diffuse0ID], colorSampler), uv);
	vec4 diffuse1 = texture(sampler2D(terrainColorTextures[diffuse1ID], colorSampler), uv);
	vec4 diffuse2 = texture(sampler2D(terrainColorTextures[diffuse2ID], colorSampler), uv);
	vec4 diffuse3 = texture(sampler2D(terrainColorTextures[diffuse3ID], colorSampler), uv);

	vec4 color = diffuse0;
	color = diffuse1 * alpha.r + (1.0 - alpha.r) * color;
	color = diffuse2 * alpha.g + (1.0 - alpha.g) * color;
	color = diffuse3 * alpha.b + (1.0 - alpha.b) * color;

	outColor = color;
}