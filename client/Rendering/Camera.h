#pragma once
#include <NovusTypes.h>

class InputManager;

class Camera
{
public:
    Camera(const vec3& pos);
    
    void Update(f32 deltaTime);

    mat4x4& GetViewMatrix() { return _viewMatrix; }

    void SetInputManager(InputManager* inputManager) { _inputManager = inputManager; }
private:
    void Rotate(f32 amount, const vec3& axis);
    void Translate(const vec3& direction);

private:
    vec3 _position;
    vec3 _direction;
    mat4x4 _viewMatrix;

    f32 _movementSpeed = 3.0f;
    f32 _rotationSpeed = Math::DegToRad(90.0f);
    InputManager* _inputManager;
    f32 _lastDeltaTime = 0.0f;
};