#pragma once

#include "scene/Scene.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace Omega {

class SceneManager {
public:
    static SceneManager& GetInstance();
    static void Shutdown();
    
    Scene* CreateScene(const std::string& name);
    void SetActiveScene(const std::string& name);
    Scene* GetActiveScene();
    Scene* GetSceneByName(const std::string& name);
    
    void Update(float deltaTime);
    void Render();
    
private:
    SceneManager();
    ~SceneManager();
    
    static SceneManager* s_Instance;
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
    Scene* m_ActiveScene;
};

} // namespace Omega
