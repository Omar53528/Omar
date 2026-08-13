#include "scene/Scene.h"

namespace Omega {

Scene::Scene() : m_Entities(), m_NextEntityId(0) {}

Scene::~Scene() {
    m_Entities.clear();
}

Entity Scene::CreateEntity(const std::string& name) {
    Entity entity(m_NextEntityId++);
    m_Entities[entity.GetId()] = std::make_shared<EntityData>(entity);
    
    if (!name.empty()) {
        m_EntityNames[name] = entity.GetId();
    }
    
    return entity;
}

void Scene::DestroyEntity(Entity entity) {
    auto it = m_Entities.find(entity.GetId());
    if (it != m_Entities.end()) {
        // Remove from names map
        for (auto& pair : m_EntityNames) {
            if (pair.second == entity.GetId()) {
                m_EntityNames.erase(pair.first);
                break;
            }
        }
        
        m_Entities.erase(it);
    }
}

size_t Scene::GetEntityCount() const {
    return m_Entities.size();
}

void Scene::Update(float deltaTime) {
    for (auto& [id, data] : m_Entities) {
        // Update components that need updating
        // This is a simplified version
    }
}

void Scene::Render() {
    // Render all entities with renderable components
}

Entity Scene::GetEntityByName(const std::string& name) {
    auto it = m_EntityNames.find(name);
    if (it != m_EntityNames.end()) {
        return Entity(it->second);
    }
    return Entity(0); // Invalid entity
}

std::vector<Entity> Scene::GetAllEntities() const {
    std::vector<Entity> entities;
    for (const auto& [id, data] : m_Entities) {
        entities.push_back(Entity(id));
    }
    return entities;
}

} // namespace Omega
