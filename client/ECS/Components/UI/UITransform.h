#pragma once
#include <NovusTypes.h>

struct UITransform
{
public:
    UITransform() : position(), localPosition(), anchor(), size(), depth(), parent(), isDirty(false) { }

    vec2 position;
    vec2 localPosition;
    vec2 anchor;
    vec2 size;
    u16 depth;
    u32 parent;
    bool isDirty;
};