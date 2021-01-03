
static float3 axis[8] =
{
    float3(0,0,0),
    float3(1,0,0),
    float3(0,1,0),
    float3(1,1,0),

    float3(0,0,1),
    float3(1,0,1),
    float3(0,1,1),
    float3(1,1,1),
};

bool IsVisible(float3 AABBMin, float3 AABBMax,float3 eye ,Texture2D<float> pyramid, SamplerState samplerState, float4x4 viewMat)
{
    if (eye.x < AABBMax.x && eye.x > AABBMin.x)
    {
        if (eye.y < AABBMax.y && eye.y > AABBMin.y)
        {
            if (eye.z < AABBMax.z && eye.z > AABBMin.z)
            {
                return true;
            }
        }
    }

    float2 pmin = float2(10000, 10000);
    float2 pmax = float2(-10000, -10000);
    float2 depth = float2(-1000, 10000);// x max, y min

    for (int i = 0; i < 8; i++)
    {
        float pX = lerp(AABBMin.x, AABBMax.x, axis[i].x);
        float pY = lerp(AABBMin.y, AABBMax.y, axis[i].y);
        float pZ = lerp(AABBMin.z, AABBMax.z, axis[i].z);

        float4 worldPoint = float4(pX, pY, pZ, 1.0);
        float4 clipPoint = mul(worldPoint, viewMat);
        clipPoint /= clipPoint.w;

        pmin.x = min(clipPoint.x, pmin.x);
        pmin.y = min(clipPoint.y, pmin.y);

        pmax.x = max(clipPoint.x, pmax.x);
        pmax.y = max(clipPoint.y, pmax.y);

        depth.x = max(clipPoint.z, depth.x);
        depth.y = min(clipPoint.z, depth.y);
    }

    //convert max and min into UV space
    pmin = pmin * float2(0.5f, -0.5f) + float2(0.5f,0.5f);
    pmax = pmax * float2(0.5f, -0.5f) + float2(0.5f,0.5f);

    uint pyrWidth;
    uint pyrHeight;
    pyramid.GetDimensions(pyrWidth, pyrHeight);

    //calculate pixel widths/height
    float boxWidth = (pmax.x-pmin.x) * pyrWidth;
    float boxHeight = (pmax.y-pmin.y) * pyrHeight;

    float level = ceil(log2(max(boxWidth, boxHeight)));

    float2 psample = (pmin + pmax) / 2;

    float sampleDepth = pyramid.SampleLevel(samplerState, psample, level).x;

    return sampleDepth <= depth.x;
};