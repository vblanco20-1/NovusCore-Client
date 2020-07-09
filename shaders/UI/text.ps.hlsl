
struct TextData
{
    float4 textColor;
    float4 outlineColor;
    float outlineWidth;
};

[[vk::binding(0, PER_PASS)]] SamplerState _sampler;

[[vk::binding(0, PER_DRAW)]] ConstantBuffer<TextData> _textData;
[[vk::binding(1, PER_DRAW)]] Texture2D<float4> _texture;

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(VertexOutput input) : SV_Target
{
    float distance = _texture.SampleLevel(_sampler, input.uv, 0).r;
    float smoothWidth = fwidth(distance);
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
    float3 rgb = float3(alpha, alpha, alpha) * _textData.textColor.rgb;

    if (_textData.outlineWidth > 0.0)
    {
        float w = 1.0 - _textData.outlineWidth;
        alpha = smoothstep(w - smoothWidth, w + smoothWidth, distance);
        rgb += lerp(float3(alpha, alpha, alpha), _textData.outlineColor.rgb, alpha);
    }

    return float4(rgb, alpha);
}