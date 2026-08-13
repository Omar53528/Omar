#pragma once

#include "core/Types.h"

namespace Omega {

class Camera3D {
public:
    Vector3 position{0.0f, 0.0f, 5.0f};
    Vector3 target{0.0f, 0.0f, 0.0f};
    Vector3 up{0.0f, 1.0f, 0.0f};
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    
    Matrix4x4 GetViewMatrix() const {
        return Matrix4x4::LookAt(position, target, up);
    }
    
    Matrix4x4 GetProjectionMatrix() const {
        return Matrix4x4::Perspective(fov, aspectRatio, nearPlane, farPlane);
    }
};

} // namespace Omega
