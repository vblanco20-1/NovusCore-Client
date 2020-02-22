#include "EngineLoop.h"
#include <Networking/InputQueue.h>
#include <Utils/Timer.h>
#include <tracy/Tracy.hpp>
#include "Utils/ServiceLocator.h"
#include "Network/MessageHandler.h"
#include "Window.h"
#include "Camera.h"
#include <Renderer.h>

// Component Singletons
#include "ECS/Components/Singletons/TimeSingleton.h"
#include "ECS/Components/Network/ConnectionSingleton.h"

// Components

// Systems
#include "ECS/Systems/Network/ConnectionSystems.h"

// Handlers
#include "Network/Handlers/Client/GeneralHandlers.h"
#include "Scripting/ScriptHandler.h"

EngineLoop::EngineLoop() : _isRunning(false), _inputQueue(256), _outputQueue(256)
{
    _network.asioService = std::make_shared<asio::io_service>(2);
    _network.client = std::make_shared<NetworkClient>(new asio::ip::tcp::socket(*_network.asioService.get()));
}

EngineLoop::~EngineLoop()
{
    delete _renderer;
}

void EngineLoop::Start()
{
    if (_isRunning)
        return;

    ServiceLocator::SetMainInputQueue(&_inputQueue);

    // Setup Network Lib
    InputQueue::SetInputQueue(&_inputQueue);

    std::thread threadRun = std::thread(&EngineLoop::Run, this);
    std::thread threadRunIoService = std::thread(&EngineLoop::RunIoService, this);
    threadRun.detach();
    threadRunIoService.detach();
}

void EngineLoop::Stop()
{
    if (!_isRunning)
        return;

    Message message;
    message.code = MSG_IN_EXIT;
    PassMessage(message);
}

void EngineLoop::PassMessage(Message& message)
{
    _inputQueue.enqueue(message);
}

bool EngineLoop::TryGetMessage(Message& message)
{
    return _outputQueue.try_dequeue(message);
}

void EngineLoop::Run()
{
    _isRunning = true;

    SetupUpdateFramework();
    _updateFramework.gameRegistry.create();

    std::string scriptPath = "./scripts";
    ScriptHandler::LoadScriptDirectory(scriptPath);

    TimeSingleton& timeSingleton = _updateFramework.gameRegistry.set<TimeSingleton>();
    ConnectionSingleton& connectionSingleton = _updateFramework.gameRegistry.set<ConnectionSingleton>();
    connectionSingleton.connection = _network.client;

    Timer timer;
    f32 targetDelta = 1.0f / 30.0f;

    _window = new Window();
    _window->Init(1920, 1080);

    _camera = new Camera(Vector3(0, 0, -5));
    _camera->SetInputManager(_window->GetInputManager());

    _renderer = new Renderer();
    _renderer->Init(_window->GetWindow());

    _network.client->Connect("127.0.0.1", 3724);

    while (true)
    {
        f32 deltaTime = timer.GetDeltaTime();
        timer.Tick();

        timeSingleton.lifeTimeInS = timer.GetLifeTime();
        timeSingleton.lifeTimeInMS = timeSingleton.lifeTimeInS * 1000;
        timeSingleton.deltaTime = deltaTime;

        if (!Update(deltaTime))
            break;

        Render();

        // Wait for tick rate, this might be an overkill implementation but it has the even tickrate I've seen - MPursche
        for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta - 0.0025f; deltaTime = timer.GetDeltaTime())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
        {
            std::this_thread::yield();
        }
    }

    // Clean up stuff here

    Message exitMessage;
    exitMessage.code = MSG_OUT_EXIT_CONFIRM;
    _outputQueue.enqueue(exitMessage);
}
void EngineLoop::RunIoService()
{
    asio::io_service::work ioWork(*_network.asioService.get());
    _network.asioService->run();
}

bool EngineLoop::Update(f32 deltaTime)
{
    bool shouldExit = _window->Update(deltaTime) == false;
    if (shouldExit)
        return false;

    Message message;
    while (_inputQueue.try_dequeue(message))
    {
        if (message.code == -1)
            assert(false);

        if (message.code == MSG_IN_EXIT)
        {
            return false;
        }
        else if (message.code == MSG_IN_PRINT)
        {
            _outputQueue.enqueue(message);
        }
        else if (message.code == MSG_IN_PING)
        {
            Message pongMessage;
            pongMessage.code = MSG_OUT_PRINT;
            pongMessage.message = new std::string("PONG!");
            _outputQueue.enqueue(pongMessage);
        }
    }

    _camera->Update(deltaTime);
    _renderer->SetViewMatrix(_camera->GetViewMatrix().Inverted());

    _renderer->RegisterRenderableCube(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0.1f, 0.1f, 0.1f));

    UpdateSystems();
    return true;
}

void EngineLoop::Render()
{
    _renderer->Render();
    _renderer->Present();
    _window->Present();
}

void EngineLoop::SetupUpdateFramework()
{
    tf::Framework& framework = _updateFramework.framework;
    entt::registry& gameRegistry = _updateFramework.gameRegistry;

    ServiceLocator::SetGameRegistry(&gameRegistry);
    SetMessageHandler();

    // ConnectionUpdateSystem
    tf::Task connectionUpdateSystemTask = framework.emplace([&gameRegistry]()
        {
            ZoneScopedNC("ConnectionUpdateSystem::Update", tracy::Color::Blue2)
                ConnectionUpdateSystem::Update(gameRegistry);
        });
}
void EngineLoop::SetMessageHandler()
{
    auto messageHandler = new MessageHandler();
    ServiceLocator::SetNetworkMessageHandler(messageHandler);

    Client::GeneralHandlers::Setup(messageHandler);
}
void EngineLoop::UpdateSystems()
{
    ZoneScopedNC("UpdateSystems", tracy::Color::Blue2)
    {
        ZoneScopedNC("Taskflow::Run", tracy::Color::Blue2)
            _updateFramework.taskflow.run(_updateFramework.framework);
    }
    {
        ZoneScopedNC("Taskflow::WaitForAll", tracy::Color::Blue2)
            _updateFramework.taskflow.wait_for_all();
    }
}
