#pragma once
#include "Opcodes.h"
#include <memory>

class NetworkClient;
struct NetworkPacket;
class MessageHandler
{
typedef bool (*MessageHandlerFn)(std::shared_ptr<NetworkClient>, NetworkPacket*);

public:
    MessageHandler();

    void SetMessageHandler(Opcode opcode, MessageHandlerFn func);
    bool CallHandler(std::shared_ptr<NetworkClient> connection, NetworkPacket* packet);

private:
    MessageHandlerFn handlers[Opcode::OPCODE_MAX_COUNT];
};