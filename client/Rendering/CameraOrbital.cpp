#include "CameraOrbital.h"
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

void CameraOrbital::Init()
{
    InputManager* inputManager = ServiceLocator::GetInputManager();
    inputManager->RegisterMousePositionCallback("CameraOrbital MouseLook", [this](Window* window, f32 xPos, f32 yPos)
    {
        if (!IsActive())
            return;

        if (_captureMouse)
        {
            vec2 mousePosition = vec2(xPos, yPos);
            if (_captureMouseHasMoved)
            {
                vec2 deltaPosition = _prevMousePosition - mousePosition;

                _yaw -= deltaPosition.x * _mouseSensitivity;
                _pitch = Math::Clamp(_pitch - (deltaPosition.y * _mouseSensitivity), -89.0f, 89.0f);
            }
            else
                _captureMouseHasMoved = true;

            _prevMousePosition = mousePosition;
        }
    });
    inputManager->RegisterKeybind("CameraOrbital Left Mouseclick", GLFW_MOUSE_BUTTON_1, KEYBIND_ACTION_CLICK, KEYBIND_MOD_ANY, [this, inputManager](Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!IsActive())
            return false;

        if (keybind->state == GLFW_PRESS)
        {
            glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            _prevMousePosition = vec2(inputManager->GetMousePositionX(), inputManager->GetMousePositionY());
        }
        else
        {
            _captureMouseHasMoved = false;
            glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        _captureMouse = !_captureMouse;
        return true;
    });

    UpdateCameraVectors();
}

void CameraOrbital::Update(f32 deltaTime, float fovInDegrees, float aspectRatioWH)
{
    // Compute matrices
    _rotationMatrix = glm::yawPitchRoll(glm::radians(_yaw), glm::radians(_pitch), 0.0f);

    const mat4x4 cameraMatrix = glm::translate(mat4x4(1.0f), _position) * _rotationMatrix;
    _viewMatrix = glm::inverse(cameraMatrix);

    _projectionMatrix = glm::perspective(glm::radians(fovInDegrees), aspectRatioWH, _nearClip, _farClip);
    _viewProjectionMatrix = _projectionMatrix * _viewMatrix;

    UpdateCameraVectors();
    UpdateFrustumPlanes(glm::transpose(_viewProjectionMatrix));
}
