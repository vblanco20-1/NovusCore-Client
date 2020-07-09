
struct Vertex
{
    float3 position;
    float2 uv;
};

[[vk::binding(0, PER_PASS)]] cbuffer ViewData
{
    float4x4 viewMatrix : packoffset(c0);
    float4x4 projMatrix : packoffset(c4);
};
[[vk::binding(0, PER_DRAW)]] cbuffer ModelData
{
    float4 colorMultiplier : packoffset(c0);
    float4x4 modelMatrix : packoffset(c1);
};
[[vk::binding(1, PER_DRAW)]] ByteAddressBuffer _vertexData;

Vertex LoadVertex(uint vertexID)
{
    Vertex vertex;
    vertex.position = asfloat(_vertexData.Load4(vertexID * 32 + 0)).xyz;
    vertex.uv = asfloat(_vertexData.Load4(vertexID * 32 + 16)).xy;
    return vertex;
}

struct VSInput
{
    uint instanceID : TEXCOORD0;
    uint vertexID : SV_VertexID;
};

struct VSOutput
{
    uint instanceID : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float4 position : SV_Position;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    uint vertexID = uint(input.vertexID) + (input.instanceID * 145u); // 145 vertices per cell

    Vertex vertex = LoadVertex(vertexID);
    output.position = mul(float4(vertex.position, 1.0f), mul(modelMatrix, mul(viewMatrix, projMatrix)));
    output.uv = vertex.uv;

    output.instanceID = input.instanceID;

    return output;
}