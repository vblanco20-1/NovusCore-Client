
struct VertexInput
{
    float3 position : POSITION;
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
    output.position = float4((input.position.xy * 2.0f) - 1.0f, input.position.z, 1.0f);
    output.uv = input.uv;
    return output;
}