#pragma once
#include <NovusTypes.h>
#include <Utils/FileReader.h>
#include <entt.hpp>

class StringTable;
class TextureLoader
{
public:
    TextureLoader() { }

    static bool Load();
    static StringTable* textureStringTable;
};