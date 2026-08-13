#include "scene/SceneManager.h"

namespace Omega {

SceneManager* SceneManager::s_Instance = nullptr;

SceneManager::SceneManager() : m_ActiveScene(nullptr) {}

SceneManager::~SceneManager() {
    m_Scenes.clear();
    m_ActiveScene = nullptr;
}

SceneManager& SceneManager::GetInstance() {
    if (!s_Instance) {
        s_Instance = new SceneManager();
    }
    return *s_Instance;
}

void SceneManager::Shutdown() {
    delete s_Instance;
    s_Instance = nullptr;
}

Scene* SceneManager::CreateScene(const std::string& name) {
    auto scene = std::make_unique<Scene>();
    m_Scenes[name] = std::move(scene);
    return m_Scenes[name].get();
}

void SceneManager::SetActiveScene(const std::string& name) {
    auto it = m_Scenes.find(name);
    if (it != m_Scenes.end()) {
        m_ActiveScene = it->second.get();
    }
}

Scene* SceneManager::GetActiveScene() {
    return m_ActiveScene;
}

Scene* SceneManager::GetSceneByName(const std::string& name) {
    auto it = m_Scenes.find(name);
    if (it != m_Scenes.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SceneManager::Update(float deltaTime) {
    if (m_ActiveScene) {
        m_ActiveScene->Update(deltaTime);
    }
}

void SceneManager::Render() {
    if (m_ActiveScene) {
        m_ActiveScene->Render();
    }
}

} // namespace Omega
