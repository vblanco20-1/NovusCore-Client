#pragma once
#include <NovusTypes.h>
#include "Camera.h"

class CameraOrbital : public Camera
{
public:
    CameraOrbital() : Camera(true) { }
    
    void Init() override;
    void Update(f32 deltaTime, float fovInDegrees, float aspectRatioWH) override;

private:
    bool _captureMouseHasMoved = false;
};