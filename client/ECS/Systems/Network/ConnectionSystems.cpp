#include "ConnectionSystems.h"
#include "../../Components/Network/ConnectionSingleton.h"
#include "../../../Network/MessageHandler.h"
#include "../../../Utils/ServiceLocator.h"
#include <tracy/Tracy.hpp>

void ConnectionUpdateSystem::Update(entt::registry& registry)
{
    MessageHandler* messageHandler = ServiceLocator::GetNetworkMessageHandler();
    ConnectionSingleton& connectionSingleton = registry.ctx<ConnectionSingleton>();

    ZoneScopedNC("InternalPacketHandlerSystem::Update", tracy::Color::Blue)

        NetworkPacket* packet;
    while (connectionSingleton.packetQueue.try_dequeue(packet))
    {
        if (!messageHandler->CallHandler(connectionSingleton.connection, packet))
        {
            connectionSingleton.packetQueue.enqueue(packet);
            continue;
        }

        delete packet;
    }
}