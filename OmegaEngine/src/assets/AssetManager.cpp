#include "assets/AssetManager.h"
#include <iostream>

namespace Omega {

AssetManager* AssetManager::s_Instance = nullptr;

AssetManager::AssetManager() {}

AssetManager::~AssetManager() {
    Clear();
}

AssetManager& AssetManager::GetInstance() {
    if (!s_Instance) {
        s_Instance = new AssetManager();
    }
    return *s_Instance;
}

void AssetManager::Shutdown() {
    delete s_Instance;
    s_Instance = nullptr;
}

Texture2D* AssetManager::LoadTexture(const std::string& name, const std::string& path) {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second.get();
    }
    
    auto texture = std::make_unique<Texture2D>();
    texture->path = path;
    Texture2D* ptr = texture.get();
    m_Textures[name] = std::move(texture);
    
    std::cout << "[AssetManager] Loaded texture: " << name << std::endl;
    return ptr;
}

Mesh* AssetManager::LoadMesh(const std::string& name, const std::string& path) {
    auto it = m_Meshes.find(name);
    if (it != m_Meshes.end()) {
        return it->second.get();
    }
    
    auto mesh = std::make_unique<Mesh>();
    Mesh* ptr = mesh.get();
    m_Meshes[name] = std::move(mesh);
    
    std::cout << "[AssetManager] Loaded mesh: " << name << std::endl;
    return ptr;
}

Shader* AssetManager::LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        return it->second.get();
    }
    
    auto shader = std::make_unique<Shader>(vertexPath, fragmentPath);
    Shader* ptr = shader.get();
    m_Shaders[name] = std::move(shader);
    
    std::cout << "[AssetManager] Loaded shader: " << name << std::endl;
    return ptr;
}

Texture2D* AssetManager::GetTexture(const std::string& name) {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second.get();
    }
    return nullptr;
}

Mesh* AssetManager::GetMesh(const std::string& name) {
    auto it = m_Meshes.find(name);
    if (it != m_Meshes.end()) {
        return it->second.get();
    }
    return nullptr;
}

Shader* AssetManager::GetShader(const std::string& name) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        return it->second.get();
    }
    return nullptr;
}

void AssetManager::Unload(const std::string& name) {
    if (m_Textures.count(name)) m_Textures.erase(name);
    if (m_Meshes.count(name)) m_Meshes.erase(name);
    if (m_Shaders.count(name)) m_Shaders.erase(name);
}

void AssetManager::Clear() {
    m_Textures.clear();
    m_Meshes.clear();
    m_Shaders.clear();
}

} // namespace Omega
