/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include <Utils/Message.h>
#include <Utils/StringUtils.h>
#include <Utils/ConcurrentQueue.h>
#include <taskflow/taskflow.hpp>
#include <entt.hpp>
#include <asio/io_service.hpp>
#include "Network/NetworkClient.h"

namespace tf
{
    class Framework;
}

struct FrameworkRegistryPair
{
    entt::registry gameRegistry;
    tf::Framework framework;
    tf::Taskflow taskflow;
};

struct NetworkPair
{
    std::shared_ptr<NetworkClient> client;
    std::shared_ptr<asio::io_service> asioService;
};

class Window;
class Renderer;
class Camera;
class EngineLoop
{
public:
    EngineLoop();
    ~EngineLoop();

    void Start();
    void Stop();

    void PassMessage(Message& message);
    bool TryGetMessage(Message& message);

    template <typename... Args>
    void PrintMessage(std::string message, Args... args)
    {
        char str[256];
        StringUtils::FormatString(str, sizeof(str), message.c_str(), args...);

        Message printMessage;
        printMessage.code = MSG_OUT_PRINT;
        printMessage.message = new std::string(str);
        _outputQueue.enqueue(printMessage);
    }

private:
    void Run();
    void RunIoService();
    bool Update(f32 deltaTime);
    void UpdateSystems();
    void Render();

    void SetupUpdateFramework();
    void SetMessageHandler();
private:
    bool _isRunning;

    moodycamel::ConcurrentQueue<Message> _inputQueue;
    moodycamel::ConcurrentQueue<Message> _outputQueue;
    FrameworkRegistryPair _updateFramework;

    Window* _window;
    Camera* _camera;
    Renderer* _renderer;
    NetworkPair _network;
};