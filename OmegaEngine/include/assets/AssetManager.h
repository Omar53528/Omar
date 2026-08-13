#pragma once

#include "graphics/Texture.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace Omega {

class AssetManager {
public:
    static AssetManager& GetInstance();
    static void Shutdown();
    
    Texture2D* LoadTexture(const std::string& name, const std::string& path);
    Mesh* LoadMesh(const std::string& name, const std::string& path);
    Shader* LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    
    Texture2D* GetTexture(const std::string& name);
    Mesh* GetMesh(const std::string& name);
    Shader* GetShader(const std::string& name);
    
    void Unload(const std::string& name);
    void Clear();
    
private:
    AssetManager();
    ~AssetManager();
    
    static AssetManager* s_Instance;
    std::unordered_map<std::string, std::unique_ptr<Texture2D>> m_Textures;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_Meshes;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;
};

} // namespace Omega
