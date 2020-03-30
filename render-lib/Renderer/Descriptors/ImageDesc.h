#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>
#include "../RenderStates.h"

namespace Renderer
{
    struct ImageDesc
    {
        std::string debugName = "";
        Vector2i dimensions = Vector2i(0, 0);
        u32 depth = 1;
        ImageFormat format = IMAGE_FORMAT_UNKNOWN;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        Vector4 clearColor = Vector4(0, 0, 0, 1); // TODO: Color class would be better here...
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(ImageID, u16);
}