#pragma once
#include <entity/fwd.hpp>

class BaseSocket;
class ConnectionUpdateSystem
{
public:
    static void Update(entt::registry& registry);

    // Handlers for Network Client
    static void AuthSocket_HandleConnect(BaseSocket* socket, bool connected);
    static void AuthSocket_HandleRead(BaseSocket* socket);
    static void AuthSocket_HandleDisconnect(BaseSocket* socket);
    static void GameSocket_HandleConnect(BaseSocket* socket, bool connected);
    static void GameSocket_HandleRead(BaseSocket* socket);
    static void GameSocket_HandleDisconnect(BaseSocket* socket);
};