#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>

namespace Renderer
{
    // Lets strong-typedef an ID type with the underlying type of u8
    STRONG_TYPEDEF(CommandListID, u8);
}