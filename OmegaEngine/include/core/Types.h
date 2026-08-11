#pragma once

#include <string>
#include <cmath>

namespace Omega {

// Vector2D للعمليات ثنائية الأبعاد
struct Vector2 {
    float x = 0.0f, y = 0.0f;
    
    Vector2() = default;
    Vector2(float _x, float _y) : x(_x), y(_y) {}
    
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }
    
    float length() const { return std::sqrt(x * x + y * y); }
    Vector2 normalized() const {
        float len = length();
        return len > 0 ? *this / len : Vector2();
    }
    
    float dot(const Vector2& other) const { return x * other.x + y * other.y; }
};

// Vector3D للعمليات ثلاثية الأبعاد
struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    
    Vector3() = default;
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }
    
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vector3 normalized() const {
        float len = length();
        return len > 0 ? *this / len : Vector3();
    }
    
    float dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

// الألوان RGBA
struct Color {
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    
    Color() = default;
    Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
    
    static Color Red() { return Color(1, 0, 0); }
    static Color Green() { return Color(0, 1, 0); }
    static Color Blue() { return Color(0, 0, 1); }
    static Color White() { return Color(1, 1, 1); }
    static Color Black() { return Color(0, 0, 0); }
};

// مستطيل للعمليات 2D
struct Rect {
    float x = 0, y = 0, width = 0, height = 0;
    
    Rect() = default;
    Rect(float _x, float _y, float _w, float _h) : x(_x), y(_y), width(_w), height(_h) {}
    
    bool contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
    
    bool intersects(const Rect& other) const {
        return x < other.x + other.width && x + width > other.x &&
               y < other.y + other.height && y + height > other.y;
    }
};

// Matrix4x4 للتحويلات 3D
class Matrix4x4 {
public:
    float data[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    
    static Matrix4x4 identity() { return Matrix4x4(); }
    
    static Matrix4x4 perspective(float fov, float aspect, float near, float far) {
        Matrix4x4 result;
        float tanHalfFov = std::tan(fov / 2.0f);
        result.data[0] = 1.0f / (aspect * tanHalfFov);
        result.data[5] = 1.0f / tanHalfFov;
        result.data[10] = -(far + near) / (far - near);
        result.data[11] = -1.0f;
        result.data[14] = -(2.0f * far * near) / (far - near);
        result.data[15] = 0.0f;
        return result;
    }
    
    static Matrix4x4 orthographic(float left, float right, float bottom, float top, float near, float far) {
        Matrix4x4 result;
        result.data[0] = 2.0f / (right - left);
        result.data[5] = 2.0f / (top - bottom);
        result.data[10] = -2.0f / (far - near);
        result.data[12] = -(right + left) / (right - left);
        result.data[13] = -(top + bottom) / (top - bottom);
        result.data[14] = -(far + near) / (far - near);
        return result;
    }
    
    static Matrix4x4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 f = (target - eye).normalized();
        Vector3 s = f.cross(up).normalized();
        Vector3 u = s.cross(f);
        
        Matrix4x4 result;
        result.data[0] = s.x; result.data[4] = s.y; result.data[8] = s.z;
        result.data[1] = u.x; result.data[5] = u.y; result.data[9] = u.z;
        result.data[2] = -f.x; result.data[6] = -f.y; result.data[10] = -f.z;
        result.data[12] = -s.dot(eye);
        result.data[13] = -u.dot(eye);
        result.data[14] = f.dot(eye);
        return result;
    }
    
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.data[i * 4 + j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.data[i * 4 + j] += data[k * 4 + j] * other.data[i * 4 + k];
                }
            }
        }
        return result;
    }
};

} // namespace Omega
