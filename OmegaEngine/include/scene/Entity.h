#pragma once

#include "core/Types.h"
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <any>

namespace Omega {

class Entity {
public:
    Entity() : m_Id(0) {}
    Entity(uint64_t id) : m_Id(id) {}
    
    uint64_t GetId() const { return m_Id; }
    bool IsValid() const { return m_Id != 0; }
    bool operator==(const Entity& other) const { return m_Id == other.m_Id; }
    bool operator!=(const Entity& other) const { return m_Id != other.m_Id; }
    
private:
    uint64_t m_Id;
};

struct EntityData {
    Entity entity;
    std::string name;
    std::unordered_map<std::string, std::any> components;
    
    EntityData(Entity e) : entity(e) {}
    
    template<typename T>
    void AddComponent(const std::string& name, const T& value) {
        components[name] = value;
    }
    
    template<typename T>
    T* GetComponent(const std::string& name) {
        auto it = components.find(name);
        if (it != components.end()) {
            try {
                return std::any_cast<T>(&it->second);
            } catch (...) {
                return nullptr;
            }
        }
        return nullptr;
    }
};

// Common component types
struct TransformComponent {
    Vector3 position{0.0f, 0.0f, 0.0f};
    Vector3 rotation{0.0f, 0.0f, 0.0f};
    Vector3 scale{1.0f, 1.0f, 1.0f};
    
    TransformComponent() = default;
    TransformComponent(const Vector3& pos) : position(pos) {}
};

struct SpriteComponent {
    Color color{1.0f, 1.0f, 1.0f, 1.0f};
    Texture2D* texture = nullptr;
    Vector2 size{1.0f, 1.0f};
};

struct CameraComponent {
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isMainCamera = true;
};

} // namespace Omega
