#include "ai/AI.h"
#include <algorithm>
#include <queue>
#include <cmath>
#include <random>

namespace Omega {

// AIPerception Implementation
void AIPerception::AddSensor(const AISensorConfig& config) {
    sensors.push_back(config);
}

void AIPerception::RemoveSensor(int index) {
    if (index >= 0 && index < (int)sensors.size()) {
        sensors.erase(sensors.begin() + index);
    }
}

std::vector<uint32_t> AIPerception::GetDetectedEntities() const {
    std::vector<uint32_t> ids;
    for (const auto& entity : detectedEntities) {
        ids.push_back(entity.id);
    }
    return ids;
}

bool AIPerception::IsEntityDetected(uint32_t entityID) const {
    return std::any_of(detectedEntities.begin(), detectedEntities.end(),
        [entityID](const DetectedEntity& e) { return e.id == entityID; });
}

Vector3 AIPerception::GetLastKnownPosition(uint32_t entityID) const {
    for (const auto& entity : detectedEntities) {
        if (entity.id == entityID) {
            return entity.lastPosition;
        }
    }
    return Vector3();
}

void AIPerception::ReportSound(const Vector3& position, float volume) {
    // Find sensors that can hear
    for (const auto& sensor : sensors) {
        if (sensor.type == PerceptionType::Hearing) {
            if (volume >= sensor.threshold) {
                // Report to detected entities
                ReportSight(0, position, volume / sensor.range);
            }
        }
    }
}

void AIPerception::ReportSight(uint32_t entityID, const Vector3& position, float visibility) {
    auto it = std::find_if(detectedEntities.begin(), detectedEntities.end(),
        [entityID](const DetectedEntity& e) { return e.id == entityID; });
    
    if (it != detectedEntities.end()) {
        it->lastPosition = position;
        it->visibility = visibility;
    } else {
        DetectedEntity entity;
        entity.id = entityID;
        entity.lastPosition = position;
        entity.visibility = visibility;
        detectedEntities.push_back(entity);
    }
}

// BTSequence Implementation
BTNodeStatus BTSequence::Execute(float deltaTime) {
    while (currentChildIndex < (int)children.size()) {
        BTNodeStatus status = children[currentChildIndex]->Execute(deltaTime);
        
        if (status == BTNodeStatus::Running) {
            return BTNodeStatus::Running;
        } else if (status == BTNodeStatus::Failure) {
            Reset();
            return BTNodeStatus::Failure;
        }
        
        currentChildIndex++;
    }
    
    Reset();
    return BTNodeStatus::Success;
}

void BTSequence::Reset() {
    currentChildIndex = 0;
    for (auto& child : children) {
        child->Reset();
    }
}

// BTSelector Implementation
BTNodeStatus BTSelector::Execute(float deltaTime) {
    while (currentChildIndex < (int)children.size()) {
        BTNodeStatus status = children[currentChildIndex]->Execute(deltaTime);
        
        if (status == BTNodeStatus::Running) {
            return BTNodeStatus::Running;
        } else if (status == BTNodeStatus::Success) {
            Reset();
            return BTNodeStatus::Success;
        }
        
        currentChildIndex++;
    }
    
    Reset();
    return BTNodeStatus::Failure;
}

void BTSelector::Reset() {
    currentChildIndex = 0;
    for (auto& child : children) {
        child->Reset();
    }
}

// BTParallel Implementation
BTNodeStatus BTParallel::Execute(float deltaTime) {
    int successCount = 0;
    int failureCount = 0;
    
    for (size_t i = 0; i < children.size(); ++i) {
        if (childStatuses.size() <= i) {
            childStatuses.push_back(BTNodeStatus::Running);
        }
        
        if (childStatuses[i] == BTNodeStatus::Running) {
            childStatuses[i] = children[i]->Execute(deltaTime);
        }
        
        if (childStatuses[i] == BTNodeStatus::Success) successCount++;
        else if (childStatuses[i] == BTNodeStatus::Failure) failureCount++;
    }
    
    switch (successPolicy) {
        case RequireOne:
            if (successCount > 0) {
                Reset();
                return BTNodeStatus::Success;
            }
            break;
        case RequireAll:
            if (failureCount > 0) {
                Reset();
                return BTNodeStatus::Failure;
            }
            if (successCount == (int)children.size()) {
                Reset();
                return BTNodeStatus::Success;
            }
            break;
        case RequireMajority:
            if (successCount > (int)children.size() / 2) {
                Reset();
                return BTNodeStatus::Success;
            }
            break;
    }
    
    return BTNodeStatus::Running;
}

void BTParallel::Reset() {
    childStatuses.clear();
    for (auto& child : children) {
        child->Reset();
    }
}

// BTInverter Implementation
BTNodeStatus BTInverter::Execute(float deltaTime) {
    if (children.empty()) return BTNodeStatus::Failure;
    
    BTNodeStatus status = children[0]->Execute(deltaTime);
    if (status == BTNodeStatus::Success) return BTNodeStatus::Failure;
    if (status == BTNodeStatus::Failure) return BTNodeStatus::Success;
    return BTNodeStatus::Running;
}

// BTSucceeder Implementation
BTNodeStatus BTSucceeder::Execute(float deltaTime) {
    if (children.empty()) return BTNodeStatus::Success;
    
    children[0]->Execute(deltaTime);
    return BTNodeStatus::Success;
}

// BTFailer Implementation
BTNodeStatus BTFailer::Execute(float deltaTime) {
    if (children.empty()) return BTNodeStatus::Failure;
    
    children[0]->Execute(deltaTime);
    return BTNodeStatus::Failure;
}

// BTRepeat Implementation
BTNodeStatus BTRepeat::Execute(float deltaTime) {
    if (children.empty()) return BTNodeStatus::Failure;
    
    BTNodeStatus status = children[0]->Execute(deltaTime);
    
    if (status == BTNodeStatus::Success) {
        currentRepeat++;
        if (maxRepeats > 0 && currentRepeat >= maxRepeats) {
            Reset();
            return BTNodeStatus::Success;
        }
        children[0]->Reset();
        return BTNodeStatus::Running;
    }
    
    return status;
}

void BTRepeat::Reset() {
    currentRepeat = 0;
    if (!children.empty()) {
        children[0]->Reset();
    }
}

// BTCondition Implementation
BTNodeStatus BTCondition::Execute(float deltaTime) {
    return conditionFunc() ? BTNodeStatus::Success : BTNodeStatus::Failure;
}

// BTAction Implementation
BTNodeStatus BTAction::Execute(float deltaTime) {
    return actionFunc(deltaTime);
}

// Blackboard Implementation
void Blackboard::SetInt(const std::string& key, int value) {
    intValues[key] = value;
}

void Blackboard::SetFloat(const std::string& key, float value) {
    floatValues[key] = value;
}

void Blackboard::SetVector3(const std::string& key, const Vector3& value) {
    vectorValues[key] = value;
}

void Blackboard::SetEntity(const std::string& key, uint32_t value) {
    entityValues[key] = value;
}

int Blackboard::GetInt(const std::string& key, int defaultValue) const {
    auto it = intValues.find(key);
    return it != intValues.end() ? it->second : defaultValue;
}

float Blackboard::GetFloat(const std::string& key, float defaultValue) const {
    auto it = floatValues.find(key);
    return it != floatValues.end() ? it->second : defaultValue;
}

Vector3 Blackboard::GetVector3(const std::string& key, const Vector3& defaultValue) const {
    auto it = vectorValues.find(key);
    return it != vectorValues.end() ? it->second : defaultValue;
}

uint32_t Blackboard::GetEntity(const std::string& key, uint32_t defaultValue) const {
    auto it = entityValues.find(key);
    return it != entityValues.end() ? it->second : defaultValue;
}

bool Blackboard::HasKey(const std::string& key) const {
    return intValues.count(key) || floatValues.count(key) || 
           vectorValues.count(key) || entityValues.count(key);
}

void Blackboard::Clear() {
    intValues.clear();
    floatValues.clear();
    vectorValues.clear();
    entityValues.clear();
}

// BehaviorTree Implementation
void BehaviorTree::SetRoot(std::unique_ptr<BTNode> root) {
    this->root = std::move(root);
}

BTNodeStatus BehaviorTree::Update(float deltaTime) {
    if (!root) return BTNodeStatus::Failure;
    return root->Execute(deltaTime);
}

void BehaviorTree::Reset() {
    if (root) root->Reset();
}

// NavMeshSystem Singleton
NavMeshSystem& NavMeshSystem::Instance() {
    static NavMeshSystem instance;
    return instance;
}

NavMeshSystem::NavMeshSystem() {}
NavMeshSystem::~NavMeshSystem() {}

void NavMeshSystem::BuildNavMesh(const std::vector<Vector3>& vertices, const std::vector<uint32_t>& indices) {
    Clear();
    
    // Simple triangle mesh to navmesh conversion
    for (size_t i = 0; i < indices.size(); i += 3) {
        NavPolygon poly;
        poly.vertices.push_back(vertices[indices[i]]);
        poly.vertices.push_back(vertices[indices[i + 1]]);
        poly.vertices.push_back(vertices[indices[i + 2]]);
        
        // Calculate normal
        Vector3 edge1 = poly.vertices[1] - poly.vertices[0];
        Vector3 edge2 = poly.vertices[2] - poly.vertices[0];
        poly.normal = edge1.Cross(edge2).Normalized();
        
        polygons.push_back(poly);
    }
}

void NavMeshSystem::Clear() {
    polygons.clear();
    agents.clear();
}

std::vector<Vector3> NavMeshSystem::FindPath(const Vector3& start, const Vector3& end) {
    std::vector<Vector3> path;
    
    auto astarPath = FindPathAStar(start, end);
    for (const auto& node : astarPath) {
        path.push_back(node.position);
    }
    
    return path;
}

bool NavMeshSystem::IsValidPosition(const Vector3& position) const {
    // Check if position is on any walkable polygon
    for (const auto& poly : polygons) {
        if (!poly.isWalkable) continue;
        
        // Simple point-in-triangle test
        // (simplified for brevity)
    }
    return false;
}

Vector3 NavMeshSystem::ProjectToNavMesh(const Vector3& position) const {
    // Find closest point on navmesh
    if (polygons.empty()) return position;
    return polygons[0].vertices[0];
}

uint32_t NavMeshSystem::CreateAgent(const NavAgent& agent) {
    uint32_t id = nextAgentID++;
    agents[id] = agent;
    return id;
}

void NavMeshSystem::DestroyAgent(uint32_t agentID) {
    agents.erase(agentID);
}

NavAgent* NavMeshSystem::GetAgent(uint32_t agentID) {
    auto it = agents.find(agentID);
    return it != agents.end() ? &it->second : nullptr;
}

void NavMeshSystem::SetAgentTarget(uint32_t agentID, const Vector3& target) {
    auto it = agents.find(agentID);
    if (it != agents.end()) {
        it->second.currentPath = FindPath(it->second.position, target);
        it->second.currentPathIndex = 0;
        it->second.isMoving = !it->second.currentPath.empty();
        it->second.pathValid = true;
    }
}

void NavMeshSystem::StopAgent(uint32_t agentID) {
    auto it = agents.find(agentID);
    if (it != agents.end()) {
        it->second.isMoving = false;
        it->second.currentPath.clear();
    }
}

void NavMeshSystem::UpdateAgents(float deltaTime) {
    for (auto& pair : agents) {
        NavAgent& agent = pair.second;
        
        if (!agent.isMoving || agent.currentPath.empty()) continue;
        
        // Move towards next waypoint
        if (agent.currentPathIndex < (int)agent.currentPath.size()) {
            Vector3 target = agent.currentPath[agent.currentPathIndex];
            Vector3 direction = target - agent.position;
            float distance = direction.Length();
            
            if (distance < agent.stoppingDistance) {
                agent.currentPathIndex++;
            } else {
                agent.velocity = direction.Normalized() * agent.speed;
                agent.position += agent.velocity * deltaTime;
            }
        } else {
            agent.isMoving = false;
        }
    }
}

std::vector<NavMeshSystem::AStarNode> NavMeshSystem::FindPathAStar(const Vector3& start, const Vector3& end) {
    std::vector<AStarNode> path;
    
    // Simplified A* - just return direct path for now
    AStarNode node;
    node.position = end;
    path.push_back(node);
    
    return path;
}

float NavMeshSystem::CalculateHeuristic(const Vector3& a, const Vector3& b) const {
    return (a - b).Length();
}

// CrowdSimulation Implementation
uint32_t CrowdSimulation::AddAgent(const CrowdAgent& agent) {
    agents.push_back(agent);
    return (uint32_t)agents.size() - 1;
}

void CrowdSimulation::RemoveAgent(uint32_t id) {
    if (id < agents.size()) {
        agents.erase(agents.begin() + id);
    }
}

void CrowdSimulation::SetGoal(uint32_t agentID, const Vector3& goal) {
    if (agentID < agents.size()) {
        agents[agentID].desiredVelocity = (goal - agents[agentID].position).Normalized() * agents[agentID].maxSpeed;
    }
}

void CrowdSimulation::AddObstacle(const Vector3& position, float radius) {
    obstacles.push_back({position, radius});
}

void CrowdSimulation::Update(float deltaTime) {
    for (auto& agent : agents) {
        Vector3 steering = Seek(agent, agent.desiredVelocity * 10.0f);
        steering += Avoid(agent);
        steering += Separate(agent);
        
        // Apply steering
        agent.velocity += steering / agent.mass;
        agent.velocity = agent.velocity.Clamped(agent.maxSpeed);
        
        // Update position
        agent.position += agent.velocity * deltaTime;
    }
}

Vector3 CrowdSimulation::Seek(CrowdAgent& agent, const Vector3& target) {
    Vector3 desired = (target - agent.position).Normalized() * agent.maxSpeed;
    return (desired - agent.velocity) * agent.maxForce;
}

Vector3 CrowdSimulation::Avoid(CrowdAgent& agent) {
    Vector3 steering;
    for (const auto& obstacle : obstacles) {
        Vector3 toObstacle = agent.position - obstacle.first;
        float distance = toObstacle.Length();
        
        if (distance < obstacle.second + agent.radius) {
            steering += toObstacle.Normalized() / distance;
        }
    }
    return steering * agent.maxForce;
}

Vector3 CrowdSimulation::Separate(const CrowdAgent& agent) {
    Vector3 steering;
    int count = 0;
    
    for (const auto& other : agents) {
        if (&other == &agent) continue;
        
        float distance = (agent.position - other.position).Length();
        if (distance > 0 && distance < agent.radius * 2.5f) {
            steering += (agent.position - other.position).Normalized() / distance;
            count++;
        }
    }
    
    if (count > 0) {
        steering /= count;
    }
    
    return steering * agent.maxForce;
}

Vector3 CrowdSimulation::Align(const CrowdAgent& agent) {
    // Placeholder
    return Vector3();
}

Vector3 CrowdSimulation::Cohere(const CrowdAgent& agent) {
    // Placeholder
    return Vector3();
}

} // namespace Omega
