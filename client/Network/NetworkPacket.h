#pragma once
#include <NovusTypes.h>
#include <Utils/ByteBuffer.h>
#include <Networking/PacketHeader.h>
#include "NetworkClient.h"

struct NetworkPacket
{
    PacketHeader header;
    std::shared_ptr<ByteBuffer> payload = nullptr;
};