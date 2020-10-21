
struct PanelData
{
    float4 color;
    uint4 slicingOffset;
    float2 dimensions;
};

[[vk::binding(0, PER_PASS)]] SamplerState _sampler;
[[vk::binding(1, PER_DRAW)]] ConstantBuffer<PanelData> _panelData;
[[vk::binding(2, PER_DRAW)]] Texture2D<float4> _texture;

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float Map(float value, float originalMin, float originalMax, float newMin, float newMax)
{
    return (value - originalMin) / (originalMax - originalMin) * (newMax - newMin) + newMin;
}

float NineSliceAxis(float coord, float pixelBorderMin, float pixelBorderMax, float scaledPixelBorderMin, float scaledPixelBorderMax)
{
    // "Min" side of axis
    if (coord < pixelBorderMin)
        return Map(coord, 0, pixelBorderMin, 0, scaledPixelBorderMin);
        
    // Middle part of axis
    if (coord < 1 - pixelBorderMax)
        return Map(coord, pixelBorderMin, 1 - pixelBorderMax, scaledPixelBorderMin, 1 - scaledPixelBorderMax);
    
    // "Max" side of axis
    return Map(coord, 1 - pixelBorderMax, 1, 1 - scaledPixelBorderMax, 1);
}

float4 main(VertexOutput input) : SV_Target
{
    float2 pixelTextureDimension; // Dimension of the actual texture, without any scaling
    _texture.GetDimensions(pixelTextureDimension.x, pixelTextureDimension.y);
    
    float2 scaledPixelTextureDimension = _panelData.dimensions; // Dimension of the scaled image in our engine
    
    float topSlicingOffset = _panelData.slicingOffset.x;
    float rightSlicingOffset = _panelData.slicingOffset.y;
    float bottomSlicingOffset = _panelData.slicingOffset.z;
    float leftSlicingOffset = _panelData.slicingOffset.w;
    
    float horizontalPixelBorderMin = leftSlicingOffset / pixelTextureDimension.x;
    float horizontalPixelBorderMax = rightSlicingOffset / pixelTextureDimension.x;
    
    float verticalPixelBorderMin = topSlicingOffset / pixelTextureDimension.y;
    float verticalPixelBorderMax = bottomSlicingOffset / pixelTextureDimension.y;

    float scaledHorizontalPixelBorderMin = leftSlicingOffset / scaledPixelTextureDimension.x;
    float scaledHorizontalPixelBorderMax = rightSlicingOffset / scaledPixelTextureDimension.x;
    
    float scaledVerticalPixelBorderMin = topSlicingOffset / scaledPixelTextureDimension.y;
    float scaledVerticalPixelBorderMax = bottomSlicingOffset / scaledPixelTextureDimension.y;

    float2 scaledUV = float2(
        NineSliceAxis(input.uv.x, scaledHorizontalPixelBorderMin, scaledHorizontalPixelBorderMax, horizontalPixelBorderMin, horizontalPixelBorderMax),
        NineSliceAxis(input.uv.y, scaledVerticalPixelBorderMin, scaledVerticalPixelBorderMax, verticalPixelBorderMin, verticalPixelBorderMax)
    );
    
    return _texture.SampleLevel(_sampler, scaledUV, 0) * _panelData.color;
}