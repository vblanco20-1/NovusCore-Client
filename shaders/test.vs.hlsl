
struct ViewData
{
    matrix viewMatrix;
    matrix projMatrix;
};

struct ModelData
{
    float4 colorMultiplier;
    matrix modelMatrix;
};

[[vk::binding(0, PER_PASS)]] ConstantBuffer<ViewData> _viewData;
[[vk::binding(0, PER_DRAW)]] ConstantBuffer<ModelData> _modelData;

struct VertexInput
{
    float4 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = mul(_viewData.projMatrix, mul(_viewData.viewMatrix, mul(_modelData.modelMatrix, input.position)));
    output.uv = input.uv;
    return output;
}