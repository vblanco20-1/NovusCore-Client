#include "CameraFreelook.h"
#include <windows.h>
#include <InputManager.h>
#include <filesystem>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <GLFW/glfw3.h>
#include <Window/Window.h>
#include <Utils/DebugHandler.h>
#include <Utils/FileReader.h>
#include "../Utils/ServiceLocator.h"
#include "../Utils/MapUtils.h"

namespace fs = std::filesystem;

void CameraFreelook::Init()
{
    _window = ServiceLocator::GetWindow();

    InputManager* inputManager = ServiceLocator::GetInputManager();
    inputManager->RegisterKeybind("CameraFreelook Forward", GLFW_KEY_W, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("CameraFreelook Backward", GLFW_KEY_S, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("CameraFreelook Left", GLFW_KEY_A, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("CameraFreelook Right", GLFW_KEY_D, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("CameraFreelook Up", GLFW_KEY_SPACE, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("CameraFreelook Down", GLFW_KEY_LEFT_CONTROL, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);

    inputManager->RegisterKeybind("CameraFreeLook ToggleMouseCapture", GLFW_KEY_TAB, KEYBIND_ACTION_PRESS, KEYBIND_MOD_ANY, [this](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        _captureMouse = !_captureMouse;

        GLFWwindow* glfwWindow = _window->GetWindow();
        if (_captureMouse)
        {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            NC_LOG_MESSAGE("Mouse captured because of tab!");
        }
        else
        {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            NC_LOG_MESSAGE("Mouse released because of tab!");
        }

        return true;
    });
    inputManager->RegisterKeybind("CameraFreeLook Right Mouseclick", GLFW_MOUSE_BUTTON_2, KEYBIND_ACTION_PRESS, KEYBIND_MOD_ANY, [this, inputManager](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        if (!_captureMouse)
        {
            _captureMouse = true;
            _prevMousePosition = vec2(inputManager->GetMousePositionX(), inputManager->GetMousePositionY());

            glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            NC_LOG_MESSAGE("Mouse captured because of mouseclick!");
        }
        return true;
    });
    inputManager->RegisterMousePositionCallback("CameraFreeLook MouseLook", [this](Window* window, f32 xPos, f32 yPos)
    {
        if (!IsActive())
            return;

        if (_captureMouse)
        {
            vec2 mousePosition = vec2(xPos, yPos);
            vec2 deltaPosition = _prevMousePosition - mousePosition;

            _yaw -= deltaPosition.x * _mouseSensitivity;
            _pitch = Math::Clamp(_pitch - (deltaPosition.y * _mouseSensitivity), -89.0f, 89.0f);

            _prevMousePosition = mousePosition;
        }
    });

    inputManager->RegisterKeybind("IncreaseCameraSpeed", GLFW_KEY_PAGE_UP, KEYBIND_ACTION_PRESS, KEYBIND_MOD_ANY, [this](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        _movementSpeed += 10.0f;
        return true;
    });
    inputManager->RegisterKeybind("DecreaseCameraSpeed", GLFW_KEY_PAGE_DOWN, KEYBIND_ACTION_PRESS, KEYBIND_MOD_ANY, [this](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        _movementSpeed = glm::max(_movementSpeed - 10.0f, 7.3333f);
        return true;
    });
    
    inputManager->RegisterKeybind("SaveCameraDefault", GLFW_KEY_F9, KEYBIND_ACTION_PRESS, KEYBIND_MOD_ANY, [this](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        SaveToFile("freelook.cameradata");
        return true;
    });  
    inputManager->RegisterKeybind("LoadCameraDefault", GLFW_KEY_F10, KEYBIND_ACTION_PRESS, KEYBIND_MOD_ANY, [this](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        LoadFromFile("freelook.cameradata");
        return true;
    });

    UpdateCameraVectors();
    ServiceLocator::SetCamera(this);
}

void CameraFreelook::Update(f32 deltaTime, float fovInDegrees, float aspectRatioWH)
{
    if (!IsActive())
        return;

    InputManager* inputManager = ServiceLocator::GetInputManager();

    // Movement
    if (inputManager->IsKeyPressed("CameraFreelook Forward"_h))
    {
        _position += _front * _movementSpeed * deltaTime;
    }
    if (inputManager->IsKeyPressed("CameraFreelook Backward"_h))
    {
        _position -= _front * _movementSpeed * deltaTime;
    }
    if (inputManager->IsKeyPressed("CameraFreelook Left"_h))
    {
        _position += _left * _movementSpeed * deltaTime;
    }
    if (inputManager->IsKeyPressed("CameraFreelook Right"_h))
    {
        _position -= _left * _movementSpeed * deltaTime;
    }
    if (inputManager->IsKeyPressed("CameraFreelook Up"_h))
    {
        _position += worldUp * _movementSpeed * deltaTime;
    }
    if (inputManager->IsKeyPressed("CameraFreelook Down"_h))
    {
        _position -= worldUp * _movementSpeed * deltaTime;
    }

    // Compute matrices
    _rotationMatrix = glm::yawPitchRoll(glm::radians(_yaw), glm::radians(_pitch), 0.0f);
    const mat4x4 cameraMatrix = glm::translate(mat4x4(1.0f), _position) * _rotationMatrix;
    _viewMatrix = glm::inverse(cameraMatrix);

    const f32 nearClip = 1.0f;
    const f32 farClip = 100000.0f;

    _projectionMatrix = glm::perspective(glm::radians(fovInDegrees), aspectRatioWH, nearClip, farClip);
    _viewProjectionMatrix = _projectionMatrix * _viewMatrix;

    UpdateCameraVectors();
    UpdateFrustumPlanes(glm::transpose(_viewProjectionMatrix));
}