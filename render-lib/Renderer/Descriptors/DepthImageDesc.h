#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>
#include "../RenderStates.h"

namespace Renderer
{
    enum DepthImageFormat
    {
        DEPTH_IMAGE_FORMAT_UNKNOWN,

        // 32-bit Z w/ Stencil
        DEPTH_IMAGE_FORMAT_D32_FLOAT_S8X24_UINT,

        // No Stencil
        DEPTH_IMAGE_FORMAT_D32_FLOAT,
        DEPTH_IMAGE_FORMAT_R32_FLOAT,

        // 24-bit Z
        DEPTH_IMAGE_FORMAT_D24_UNORM_S8_UINT,

        // 16-bit Z w/o Stencil
        DEPTH_IMAGE_FORMAT_D16_UNORM,
        DEPTH_IMAGE_FORMAT_R16_UNORM
    };

    struct DepthImageDesc
    {
        std::string debugName = "";
        Vector2i dimensions = Vector2i(0,0);
        DepthImageFormat format = DEPTH_IMAGE_FORMAT_UNKNOWN;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        f32 depthClearValue = 1.0f;
        u8 stencilClearValue = 0;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(DepthImageID, u16);
}