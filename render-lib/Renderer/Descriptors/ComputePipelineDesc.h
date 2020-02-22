#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>

#include "ComputeShaderDesc.h"

namespace Renderer
{
    struct ComputePipelineDesc
    {
        ComputeShaderID computeShader = ComputeShaderID::Invalid();
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(ComputePipelineID, u16);
}