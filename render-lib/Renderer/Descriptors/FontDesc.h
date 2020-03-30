#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>

namespace Renderer
{
    struct FontDesc
    {
        std::string path = "";
        float size = 0.0f;
        int padding = 3;
    };
}