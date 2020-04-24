#pragma once
#include <NovusTypes.h>

// We need to define structs for event data, so we can pass data into callbacks for angelscript
struct UITransformEvents
{
public:
    UITransformEvents() : flags() { }

    u32 flags;
};