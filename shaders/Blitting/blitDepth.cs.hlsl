
[[vk::binding(2, 0)]] RWTexture2D<float> _target;
[[vk::binding(0, 0)]] SamplerState _sampler;
[[vk::binding(1, 0)]] Texture2D<float> _source;


struct Constants
{
    float2 imageSize;
    uint level;
    uint dummy;
};

[[vk::push_constant]] Constants _constants;


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{     
    //uint2 sourceDim;
    //_source.GetDimensions(sourceDim.x, sourceDim.y);

    float2 uvOffset = float2(0.5, 0.5);
    //if (_constants.level == 0)
    //{
    //    uvOffset = float2(0, 0);
    //}
    float2 uv = (float2(DTid.xy) + uvOffset) / _constants.imageSize;

    float depth = _source.SampleLevel(_sampler, uv, 0).x;
    //hole filling
    //[branch]
    //if ((_constants.level == 1 /*|| _constants.level == 2*/) && depth == 0)
    if(false)
    {
        int3 pxuv = int3(DTid.x * 2, DTid.y * 2,0);
       
        float d0 = _source.Load(pxuv + int3(0, 0,0));
        float d1 = _source.Load(pxuv + int3(1, 0,0));
        float d2 = _source.Load(pxuv + int3(0, 1,0));
        float d3 = _source.Load(pxuv + int3(1, 1,0));

        if (d0 != 0)
        {
            depth = d0;
        }
        if (d1 != 0)
        {
            depth = min(depth,d1);
        }
        if (d2 != 0)
        {
            depth = min(depth, d2);
        }
        if (d3 != 0)
        {
            depth = min(depth, d3);
        }
    }
    _target[DTid.xy] = depth;
}