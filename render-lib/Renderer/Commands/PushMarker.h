#pragma once
#include <NovusTypes.h>

namespace Renderer
{
    namespace Commands
    {
        struct PushMarker
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            Vector3 color = Vector3(1, 1, 1);
            char marker[16];
        };
    }
}