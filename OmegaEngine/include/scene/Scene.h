#pragma once

#include "scene/Entity.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace Omega {

class Scene {
public:
    Scene();
    ~Scene();
    
    Entity CreateEntity(const std::string& name = "");
    void DestroyEntity(Entity entity);
    
    size_t GetEntityCount() const;
    
    void Update(float deltaTime);
    void Render();
    
    Entity GetEntityByName(const std::string& name);
    std::vector<Entity> GetAllEntities() const;
    
private:
    std::unordered_map<uint64_t, std::shared_ptr<EntityData>> m_Entities;
    std::unordered_map<std::string, uint64_t> m_EntityNames;
    uint64_t m_NextEntityId;
};

} // namespace Omega
