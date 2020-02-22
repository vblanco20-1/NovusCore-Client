#pragma once
#include <NovusTypes.h>
#include <Utils/ConcurrentQueue.h>
#include "../../../Network/NetworkClient.h"
#include "../../../Network/NetworkPacket.h"

struct ConnectionSingleton
{
public:
    ConnectionSingleton() : packetQueue(256) { }

    std::shared_ptr<NetworkClient> connection;
    moodycamel::ConcurrentQueue<NetworkPacket*> packetQueue;
};