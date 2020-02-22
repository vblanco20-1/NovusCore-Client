#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>

namespace Renderer
{
    struct VertexShaderDesc
    {
        std::string path;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(VertexShaderID, u16);
}