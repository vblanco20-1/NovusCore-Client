#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>
#include "../RenderStates.h"

namespace Renderer
{
    struct TextureDesc
    {
        std::string path = "";
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(TextureID, u16);
}