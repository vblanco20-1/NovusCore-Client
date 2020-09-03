#include "globalData.inc.hlsl"
#include "terrain.inc.hlsl"

[[vk::binding(0, PER_PASS)]] ByteAddressBuffer _vertexHeights;
[[vk::binding(1, PER_PASS)]] ByteAddressBuffer _cellDataVS;

struct VSInput
{
    uint vertexID : SV_VertexID;
    uint packedChunkCellID : TEXCOORD0;
    uint cellIndex : TEXCOORD1;
};

struct VSOutput
{
    float4 position : SV_Position;
    uint packedChunkCellID : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 normal : TEXCOORD2;
    uint cellIndex : TEXCOORD3;
};

struct Vertex
{
    float3 position;
    float3 normal;
    float2 uv;
};

Vertex LoadVertex(uint chunkID, uint cellID, uint vertexBaseOffset, uint vertexID)
{
    // Load height
    const uint heightIndex = vertexBaseOffset + vertexID;
    const float height = _vertexHeights.Load<float>(heightIndex * 4); // 4 = sizeof(float)

    float2 cellPos = GetCellPosition(chunkID, cellID);
    float2 vertexPos = GetCellSpaceVertexPosition(vertexID);

    const float CELL_PRECISION = CELL_SIDE_SIZE / 8.0f;

    Vertex vertex;
    vertex.position.x = -((-vertexPos.x) * CELL_PRECISION + cellPos.x);
    vertex.position.y = height;
    vertex.position.z = (-vertexPos.y) * CELL_PRECISION + cellPos.y;

    // TODO: Calculate normal
    vertex.normal = float3(0, 1, 0);
    vertex.uv = vertexPos;

    vertex.position = mul(float3x3(
         0, 0, 1,
         0, 1, 0,
        -1, 0, 0
    ), vertex.position);

    return vertex;
}

CellData LoadCellData(uint globalCellID)
{
    const PackedCellData rawCellData = _cellDataVS.Load<PackedCellData>(globalCellID * 12); // sizeof(PackedCellData) = 12

    CellData cellData;

    // Unpack diffuse IDs
    cellData.diffuseIDs.x = (rawCellData.packedDiffuseIDs1 >> 0) & 0xffff;
    cellData.diffuseIDs.y = (rawCellData.packedDiffuseIDs1 >> 16) & 0xffff;
    cellData.diffuseIDs.z = (rawCellData.packedDiffuseIDs2 >> 0) & 0xffff;
    cellData.diffuseIDs.w = (rawCellData.packedDiffuseIDs2 >> 16) & 0xffff;

    // Unpack holes
    cellData.holes = rawCellData.packedHoles & 0xffff;

    return cellData;
}

VSOutput main(VSInput input)
{
    VSOutput output;

    CellData cellData = LoadCellData(input.cellIndex);
    if (IsHoleVertex(input.vertexID, cellData.holes))
    {
        const float NaN = asfloat(0b01111111100000000000000000000000);
        output.position = float4(NaN, NaN, NaN, NaN);
        return output;
    }

    const uint cellID = input.packedChunkCellID & 0xffff;
    const uint chunkID = input.packedChunkCellID >> 16;

    uint vertexBaseOffset = input.cellIndex * NUM_VERTICES_PER_CELL;
    Vertex vertex = LoadVertex(chunkID, cellID, vertexBaseOffset, input.vertexID);

    output.position = mul(float4(vertex.position, 1.0f), _viewData.viewProjectionMatrix);
    output.uv = vertex.uv;
    output.packedChunkCellID = input.packedChunkCellID;
    output.cellIndex = input.cellIndex;

    return output;
}