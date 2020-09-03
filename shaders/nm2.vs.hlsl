#include "globalData.inc.hlsl"

[[vk::binding(0, PER_PASS)]] ByteAddressBuffer _instanceData;

[[vk::binding(0, PER_DRAW)]] ByteAddressBuffer _vertexPositions;
[[vk::binding(1, PER_DRAW)]] ByteAddressBuffer _vertexUVs;

struct InstanceData
{
    float4x4 instanceMatrix;
};

struct Vertex
{
    float3 position;
    float2 uv;
};

struct VSInput
{
    uint vertexID : SV_VertexID;
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

InstanceData LoadInstanceData(uint instanceID)
{
    InstanceData instanceData;

    instanceData = _instanceData.Load<InstanceData>(instanceID * 24); // 24 = sizeof(InstanceData)

    return instanceData;
}

Vertex LoadVertex(uint vertexID)
{
    Vertex vertex;

    vertex.position = _vertexPositions.Load<float3>(vertexID * 12); // 12 = sizeof(float3)
    vertex.uv = _vertexUVs.Load<float2>(vertexID * 8); // 8 = sizeof(float2)

    // TODO: Remove this from the shader, we want to do this in the dataextractor instead
    vertex.position = float3(-vertex.position.x, vertex.position.z, -vertex.position.y);

    return vertex;
}

VSOutput main(VSInput input)
{
    VSOutput output;

    InstanceData instanceData = LoadInstanceData(input.instanceID);
    Vertex vertex = LoadVertex(input.vertexID); 

    float4 position = float4(vertex.position, 1.0f);
    position = mul(position, instanceData.instanceMatrix);

    output.position = mul(position, _viewData.viewProjectionMatrix);
    output.uv = vertex.uv;

    return output;
}