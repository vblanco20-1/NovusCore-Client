#include "ServiceLocator.h"

entt::registry * ServiceLocator::_gameRegistry = nullptr;
MessageHandler* ServiceLocator::_networkMessageHandler = nullptr;

moodycamel::ConcurrentQueue<Message>* ServiceLocator::_mainInputQueue = nullptr;

void ServiceLocator::SetGameRegistry(entt::registry* registry)
{
    assert(_gameRegistry == nullptr);
    _gameRegistry = registry;
}
void ServiceLocator::SetNetworkMessageHandler(MessageHandler* networkMessageHandler)
{
    assert(_networkMessageHandler == nullptr);
    _networkMessageHandler = networkMessageHandler;
}