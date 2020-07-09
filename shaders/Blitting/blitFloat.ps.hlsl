
[[vk::binding(0, 0)]] SamplerState _sampler;
[[vk::binding(1, 0)]] Texture2D<float4> _texture;

struct VSOutput
{
    float2 uv : TEXCOORD0;
};

float4 main(VSOutput input) : SV_Target
{
    return _texture.Sample(_sampler, input.uv);
}