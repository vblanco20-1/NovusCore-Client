#pragma once

class MessageHandler;
struct Packet;
namespace Client
{
    class GeneralHandlers
    {
    public:
        static void Setup(MessageHandler*);
    };
}