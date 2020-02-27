#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>
#include "../RenderStates.h"

namespace Renderer
{
    typedef Sampler SamplerDesc;

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(SamplerID, u16);
}