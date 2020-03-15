#pragma once
#include <Utils/ConcurrentQueue.h>
#include <Utils/Message.h>
#include <entt.hpp>
#include <cassert>

class MessageHandler;
class Window;
class InputManager;
class ServiceLocator
{
public:
    static entt::registry* GetGameRegistry() { return _gameRegistry; }
    static void SetGameRegistry(entt::registry* registry);
    static MessageHandler* GetNetworkMessageHandler() { return _networkMessageHandler; }
    static void SetNetworkMessageHandler(MessageHandler* serverMessageHandler);
    static Window* GetWindow() { return _window; }
    static void SetWindow(Window* window);
    static InputManager* GetInputManager() { return _inputManager; }
    static void SetInputManager(InputManager* inputManager);
    static moodycamel::ConcurrentQueue<Message>* GetMainInputQueue() 
    {
        assert(_mainInputQueue != nullptr);
        return _mainInputQueue; 
    }
    static void SetMainInputQueue(moodycamel::ConcurrentQueue<Message>* mainInputQueue)
    {
        assert(_mainInputQueue == nullptr);
        _mainInputQueue = mainInputQueue;
    }

private:
    ServiceLocator() { }
    static entt::registry* _gameRegistry;
    static MessageHandler* _networkMessageHandler;
    static Window* _window;
    static InputManager* _inputManager;
    static moodycamel::ConcurrentQueue<Message>* _mainInputQueue;
};