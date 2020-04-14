#pragma once
#include <NovusTypes.h>

namespace Renderer
{
    namespace Commands
    {
        struct PushMarker
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            Color color = Color::White;
            char marker[16];
        };
    }
}