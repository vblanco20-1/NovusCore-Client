
[[vk::binding(1, PER_PASS)]] SamplerState _sampler;
[[vk::binding(2, PER_PASS)]] ByteAddressBuffer _materialData;
[[vk::binding(3, PER_PASS)]] Texture2D<float4> _textures[1024];

struct MaterialParam
{
    uint materialID;
};

[[vk::push_constant]] MaterialParam materialParam;

struct Material
{
    uint flag;
    uint textureIDs[4];
};

struct PSInput
{
    float2 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

Material LoadMaterial()
{
    Material material;
    material = _materialData.Load<Material>(materialParam.materialID * 20); // 20 = sizeof(Material)

    return material;
}

PSOutput main(PSInput input)
{
    PSOutput output;

    Material material = LoadMaterial();

    float4 color0 = _textures[material.textureIDs[0]].Sample(_sampler, input.uv);
    output.color = float4(color0.xyz, 1);

    return output;
}