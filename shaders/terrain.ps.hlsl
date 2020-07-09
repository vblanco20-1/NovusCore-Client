
struct ChunkData
{
    uint4 diffuseIDs;
};

[[vk::binding(1, PER_PASS)]] SamplerState _alphaSampler;
[[vk::binding(2, PER_PASS)]] SamplerState _colorSampler;

[[vk::binding(3, PER_PASS)]] Texture2D<float4> _terrainColorTextures[4096];
[[vk::binding(4, PER_PASS)]] Texture2DArray<float4> _terrainAlphaTextures[196];

[[vk::binding(2, PER_DRAW)]] cbuffer ChunkData
{
    ChunkData _chunkDatas[256] : packoffset(c0);
};

struct PSInput
{
    nointerpolation uint instanceID : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output;

    uint instanceID = input.instanceID;
    
    // Our UVs currently go between 0 and 8, with wrapping. This is correct for terrain color textures
    float2 uv = input.uv; // [0.0 .. 8.0]

    // However the alpha needs to be between 0 and 1, so lets convert it
    float3 alphaUV = float3(uv / 8.0f.xx, float(instanceID));

    // We have 4 uints per chunk for our diffuseIDs, this gives us a size and alignment of 16 bytes which is exactly what GPUs want
    // However, we need a fifth uint for alphaID, so we decided to pack it into the LAST diffuseID, which gets split into two uint16s
    // This is what it looks like
    // [1111] diffuseIDs.x
    // [2222] diffuseIDs.y
    // [3333] diffuseIDs.z
    // [AA44] diffuseIDs.w Alpha is read from the most significant bits, the fourth diffuseID read from the least 
    uint diffuse0ID = _chunkDatas[instanceID].diffuseIDs.x;
    uint diffuse1ID = _chunkDatas[instanceID].diffuseIDs.y;
    uint diffuse2ID = _chunkDatas[instanceID].diffuseIDs.z;
    uint diffuse3ID = _chunkDatas[instanceID].diffuseIDs.w & 65535u;
    uint alphaID = _chunkDatas[instanceID].diffuseIDs.w >> uint(16);

    float3 alpha = _terrainAlphaTextures[alphaID].Sample(_alphaSampler, alphaUV).rgb;
    float4 diffuse0 = _terrainColorTextures[diffuse0ID].Sample(_colorSampler, uv);
    float4 diffuse1 = _terrainColorTextures[diffuse1ID].Sample(_colorSampler, uv);
    float4 diffuse2 = _terrainColorTextures[diffuse2ID].Sample(_colorSampler, uv);
    float4 diffuse3 = _terrainColorTextures[diffuse3ID].Sample(_colorSampler, uv);
    float4 color = diffuse0;
    color = (diffuse1 * alpha.x) + (color * (1.0f - alpha.x));
    color = (diffuse2 * alpha.y) + (color * (1.0f - alpha.y));
    color = (diffuse3 * alpha.z) + (color * (1.0f - alpha.z));
    output.color = color;

    return output;
}