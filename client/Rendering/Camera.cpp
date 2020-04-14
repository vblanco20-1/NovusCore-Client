#include "Camera.h"
#include <windows.h>
#include <InputManager.h>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(const vec3& pos)
    : _viewMatrix()
    , _direction(0,0,1)
{
    _position = pos;
}

void Camera::Update(f32 deltaTime)
{
    _lastDeltaTime = deltaTime;

    // Movement
    if (GetAsyncKeyState('W'))
    {
        vec3 direction = _direction;
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (GetAsyncKeyState('S'))
    {
        vec3 direction = -_direction;
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (GetAsyncKeyState('A'))
    {
        vec3 direction = glm::cross(_direction, vec3(0, 1, 0));
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (GetAsyncKeyState('D'))
    {
        vec3 direction = -glm::cross(_direction, vec3(0, 1, 0));
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (GetAsyncKeyState(VK_SPACE))
    {
        vec3 direction = vec3(0, 1, 0);
        Translate(direction * _movementSpeed * deltaTime);
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
        vec3 direction = vec3(0, -1, 0);
        Translate(direction * _movementSpeed * deltaTime);
    }

    const vec3& forward = _direction;
    const vec3& up = vec3(0, 1, 0);
    const vec3& right = glm::cross(forward, up);

    // Rotation
    if (GetAsyncKeyState('Q'))
    {
        Rotate(-_rotationSpeed * deltaTime, forward);
    }
    if (GetAsyncKeyState('E'))
    {
        Rotate(_rotationSpeed * deltaTime, forward);
    }
    if (GetAsyncKeyState(VK_UP))
    {
        Rotate(-_rotationSpeed * deltaTime, right);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        Rotate(_rotationSpeed * deltaTime, right);
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        Rotate(-_rotationSpeed * deltaTime, up);
    }
    if (GetAsyncKeyState(VK_RIGHT))
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
