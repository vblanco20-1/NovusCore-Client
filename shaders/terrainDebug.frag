#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension VK_EXT_descriptor_indexing : enable
#extension GL_EXT_scalar_block_layout : enable

// Textures
layout(set = 3, binding = 0) uniform sampler alphaSampler;
layout(set = 4, binding = 0) uniform sampler colorSampler;
layout(set = 5, binding = 0) uniform texture2D terrainTextures[4096];
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

layout(location = 0) out uvec4 outTextureID;
layout(location = 1) out vec4 outAlphaMap;

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
	uint diffuse3ID = (chunkDatas[fragInstanceID].diffuseIDs[3] & 0xFFFF);
	uint alphaID	= (chunkDatas[fragInstanceID].diffuseIDs[3] & 0xFFFF0000) >> 16;
	
	vec3 alphaBlend = texture(sampler2DArray(terrainAlphaTextures[alphaID], alphaSampler), alphaUV).rgb;

	/*float minusAlphaBlendSum = (1.0 - clamp(dot(alphaBlend, vec3(1.0)), 0.0, 1.0));
	vec4 weightsVector = vec4(minusAlphaBlendSum, alphaBlend);
	vec4 weightsTemp = (weightsVector * (vec4(1.0) - clamp((vec4(max(max(weightsVector.x, weightsVector.y), max(weightsVector.z, weightsVector.w))) - weightsVector), 0.0, 1.0)));

	vec4 weightsNormalized = (weightsTemp / vec4(dot(vec4(1.0), weightsTemp)));

	vec4 diffuse0 = texture(sampler2D(terrainTextures[diffuse0ID], terrainSampler), uv) * weightsNormalized.x;
	outColor = diffuse0;
	
	vec4 diffuse1 = texture(sampler2D(terrainTextures[diffuse1ID], terrainSampler), uv) * weightsNormalized.y;
	outColor += diffuse1;

	vec4 diffuse2 = texture(sampler2D(terrainTextures[diffuse2ID], terrainSampler), uv) * weightsNormalized.z;
	outColor += diffuse2;

	vec4 diffuse3 = texture(sampler2D(terrainTextures[diffuse3ID], terrainSampler), uv) * weightsNormalized.w;
	outColor += diffuse3;*/

	outTextureID = uvec4(diffuse0ID, diffuse1ID, diffuse2ID, diffuse3ID);
	outAlphaMap = vec4(alphaBlend, 1.0);
}