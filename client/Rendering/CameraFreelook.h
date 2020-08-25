#pragma once
#include <NovusTypes.h>
#include "Camera.h"

class Window;
class CameraFreelook : public Camera
{
public:
    CameraFreelook(const vec3& pos) : Camera(true) { _position = pos; }
    
    void Init() override;
    void Update(f32 deltaTime, float fovInDegrees, float aspectRatioWH) override;

private:
    Window* _window = nullptr;
};