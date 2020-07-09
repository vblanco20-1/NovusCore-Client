
[[vk::binding(0, 0)]] SamplerState _sampler;
[[vk::binding(1, 0)]] Texture2D<int4> _texture;

struct VSOutput
{
    float2 uv : TEXCOORD0;
};

float4 main(VSOutput input) : SV_Target
{
    float2 dimensions;
    _texture.GetDimensions(dimensions.x, dimensions.y);

    int3 location = int3(input.uv * dimensions, 0);

    return _texture.Load(location) / 127.0;
}