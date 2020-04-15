#include "Camera.h"
#include <windows.h>
#include <InputManager.h>
#include "../Utils/ServiceLocator.h"
#include <glm/gtx/rotate_vector.hpp>
#include <GLFW/glfw3.h>

Camera::Camera(const vec3& pos)
    : _viewMatrix()
    , _direction(0,0,1)
{
    _position = pos;
}

void Camera::Init()
{
    InputManager* inputManager = ServiceLocator::GetInputManager();

    inputManager->RegisterKeybind("Camera Forward", GLFW_KEY_W, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Backward", GLFW_KEY_S, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Left", GLFW_KEY_A, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Right", GLFW_KEY_D, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Up", GLFW_KEY_SPACE, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Down", GLFW_KEY_LEFT_SHIFT, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);


    inputManager->RegisterKeybind("Camera Rotate Up", GLFW_KEY_DOWN, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Rotate Down", GLFW_KEY_UP, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Rotate Left", GLFW_KEY_LEFT, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
    inputManager->RegisterKeybind("Camera Rotate Right", GLFW_KEY_RIGHT, KEYBIND_ACTION_PRESS | KEYBIND_ACTION_REPEAT, KEYBIND_MOD_ANY);
}

void Camera::Update(f32 deltaTime)
{
    InputManager* inputManager = ServiceLocator::GetInputManager();
    _lastDeltaTime = deltaTime;

    // Movement
    if (inputManager->IsKeyPressed("Camera Forward"_h))
    {
        vec3 direction = _direction;
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsKeyPressed("Camera Backward"_h))
    {
        vec3 direction = -_direction;
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsKeyPressed("Camera Left"_h))
    {
        vec3 direction = glm::cross(_direction, vec3(0, 1, 0));
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsKeyPressed("Camera Right"_h))
    {
        vec3 direction = -glm::cross(_direction, vec3(0, 1, 0));
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsKeyPressed("Camera Up"_h))
    {
        vec3 direction = vec3(0, 1, 0);
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsKeyPressed("Camera Down"_h))
    {
        vec3 direction = vec3(0, -1, 0);
        Translate(direction * _movementSpeed * deltaTime);
    }

    const vec3& forward = _direction;
    const vec3& up = vec3(0, 1, 0);
    const vec3& right = glm::cross(forward, up);

    // Rotation

    if (inputManager->IsKeyPressed("Camera Rotate Up"_h))
    {
        Rotate(_rotationSpeed * deltaTime, right);
    }
    if (inputManager->IsKeyPressed("Camera Rotate Down"_h))
    {
        Rotate(-_rotationSpeed * deltaTime, right);
    }
    if (inputManager->IsKeyPressed("Camera Rotate Left"_h))
    {
        Rotate(-_rotationSpeed * deltaTime, up);
    }
    if (inputManager->IsKeyPressed("Camera Rotate Right"_h))
    {
        Rotate(_rotationSpeed * deltaTime, up);
    }

    _viewMatrix = glm::lookAt(_position, _position + _direction, up);
}

void Camera::Rotate(f32 amount, const vec3& axis)
{
    _direction = glm::rotate(_direction, amount, axis);
}

void Camera::Translate(const vec3& direction)
{
    _position += direction;
}