#pragma once
#include <NovusTypes.h>
#include "../Descriptors/TextureDesc.h"
#include "../Descriptors/SamplerDesc.h"

namespace Renderer
{
    namespace Commands
    {
        struct SetTextureSampler
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            u32 slot = 0;
            TextureID texture = TextureID::Invalid();
            SamplerID sampler = SamplerID::Invalid();
        };
    }
}