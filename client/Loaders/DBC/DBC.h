#pragma once
#include <NovusTypes.h>
#include <Utils/ByteBuffer.h>
#include <Containers/StringTable.h>

namespace DBC
{
    constexpr i32 DBC_TOKEN = 1313096259;
    constexpr i32 DBC_VERSION = 1;

    struct DBCHeader
    {
        u32 token;
        u32 version;
    };

    struct File
    {
        DBCHeader header;
        std::shared_ptr<Bytebuffer> buffer;
    };

    struct Map
    {
        u32 Id = 0;
        u32 Name = 0;
        u32 InternalName = 0;
        u32 InstanceType = 0;
        u32 Flags = 0;
        u32 Expansion = 0;
        u32 MaxPlayers = 0;
    };
}