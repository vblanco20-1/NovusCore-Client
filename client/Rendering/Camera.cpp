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

    inputManager->RegisterBinding("Camera Forward", GLFW_KEY_W, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Backward", GLFW_KEY_S, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Left", GLFW_KEY_A, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Right", GLFW_KEY_D, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Up", GLFW_KEY_SPACE, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Down", GLFW_KEY_LEFT_SHIFT, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);


    inputManager->RegisterBinding("Camera Rotate Up", GLFW_KEY_DOWN, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Rotate Down", GLFW_KEY_UP, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Rotate Left", GLFW_KEY_LEFT, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
    inputManager->RegisterBinding("Camera Rotate Right", GLFW_KEY_RIGHT, BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT, BINDING_MOD_ANY);
}

void Camera::Update(f32 deltaTime)
{
    InputManager* inputManager = ServiceLocator::GetInputManager();
    _lastDeltaTime = deltaTime;

    // Movement
    if (inputManager->IsPressed("Camera Forward"_h))
    {
        vec3 direction = _direction;
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsPressed("Camera Backward"_h))
    {
        vec3 direction = -_direction;
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsPressed("Camera Left"_h))
    {
        vec3 direction = glm::cross(_direction, vec3(0, 1, 0));
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsPressed("Camera Right"_h))
    {
        vec3 direction = -glm::cross(_direction, vec3(0, 1, 0));
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsPressed("Camera Up"_h))
    {
        vec3 direction = vec3(0, 1, 0);
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (inputManager->IsPressed("Camera Down"_h))
    {
        vec3 direction = vec3(0, -1, 0);
        Translate(direction * _movementSpeed * deltaTime);
    }

    const vec3& forward = _direction;
    const vec3& up = vec3(0, 1, 0);
    const vec3& right = glm::cross(forward, up);

    // Rotation

    if (inputManager->IsPressed("Camera Rotate Up"_h))
    {
        Rotate(_rotationSpeed * deltaTime, right);
    }
    if (inputManager->IsPressed("Camera Rotate Down"_h))
    {
        Rotate(-_rotationSpeed * deltaTime, right);
    }
    if (inputManager->IsPressed("Camera Rotate Left"_h))
    {
        Rotate(-_rotationSpeed * deltaTime, up);
    }
    if (inputManager->IsPressed("Camera Rotate Right"_h))
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