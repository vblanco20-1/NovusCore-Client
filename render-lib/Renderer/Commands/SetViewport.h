#pragma once
#include <NovusTypes.h>
#include "../RenderStates.h"

namespace Renderer
{
    namespace Commands
    {
        struct SetViewport
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            Viewport viewport;
        };
    }
}