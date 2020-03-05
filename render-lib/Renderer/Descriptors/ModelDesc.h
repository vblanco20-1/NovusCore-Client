#pragma once
#include <NovusTypes.h>
#include <Utils/StrongTypedef.h>
#include <vector>

namespace Renderer
{
    struct Vertex
    {
        Vector3 pos;
        Vector3 normal;
        Vector2 texCoord;
    };

    struct ModelDesc
    {
        std::string path;
    };

    struct PrimitiveModelDesc
    {
        std::vector<Vertex> vertices;
        std::vector<i16> indices;
    };

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(ModelID, u16);
}