#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>
#include "../RenderStates.h"

namespace Renderer
{
    struct ImageDesc
    {
        std::string debugName = "";
        ivec2 dimensions = ivec2(0, 0);
        u32 depth = 1;
        ImageFormat format = IMAGE_FORMAT_UNKNOWN;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        Color clearColor = Color::Clear;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(ImageID, u16);
}