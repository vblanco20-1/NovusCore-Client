
[[vk::binding(2, PER_PASS)]] SamplerState _sampler;

struct PSInput
{
    float2 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    output.color = float4(input.uv, 0, 1);

    return output;
}