#include "ServiceLocator.h"

entt::registry* ServiceLocator::_gameRegistry = nullptr;
MessageHandler* ServiceLocator::_networkMessageHandler = nullptr;
Window* ServiceLocator::_window = nullptr;
InputManager* ServiceLocator::_inputManager = nullptr;

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
void ServiceLocator::SetWindow(Window* window)
{
    assert(_window == nullptr);
    _window = window;
}

void ServiceLocator::SetInputManager(InputManager* inputManager)
{
    assert(_inputManager == nullptr);
    _inputManager = inputManager;
}
