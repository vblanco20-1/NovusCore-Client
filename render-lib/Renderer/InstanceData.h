#pragma once
#include <NovusTypes.h>

namespace Renderer
{
    // This struct needs to be 256 bytes padded
    struct InstanceData
    {
        Vector4 colorMultiplier = Vector4(1,1,1,1); // 16 bytes
        Matrix modelMatrix = Matrix(); // 64 bytes
        u8 padding[176];
    };
}