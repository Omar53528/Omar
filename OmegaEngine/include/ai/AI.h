#pragma once

#include "core/Types.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace Omega {

// AI Perception Types
enum class PerceptionType {
    Vision,
    Hearing,
    Touch,
    Smell
};

struct AISensorConfig {
    PerceptionType type = PerceptionType::Vision;
    float range = 10.0f;
    float angle = 90.0f; // For vision (degrees)
    float threshold = 0.5f;
    std::vector<std::string> detectableTags;
};

// AI Perception Component
class AIPerception {
public:
    void AddSensor(const AISensorConfig& config);
    void RemoveSensor(int index);
    
    // Query detected entities
    std::vector<uint32_t> GetDetectedEntities() const;
    bool IsEntityDetected(uint32_t entityID) const;
    Vector3 GetLastKnownPosition(uint32_t entityID) const;
    
    // Stimulus
    void ReportSound(const Vector3& position, float volume);
    void ReportSight(uint32_t entityID, const Vector3& position, float visibility);
    
private:
    std::vector<AISensorConfig> sensors;
    struct DetectedEntity {
        uint32_t id;
        Vector3 lastPosition;
        float lastSeenTime;
        float visibility;
    };
    std::vector<DetectedEntity> detectedEntities;
};

// Behavior Tree Node
enum class BTNodeStatus {
    Success,
    Failure,
    Running
};

class BTNode {
public:
    virtual ~BTNode() = default;
    virtual BTNodeStatus Execute(float deltaTime) = 0;
    virtual void Reset() {}
    
    std::string name;
    std::vector<std::unique_ptr<BTNode>> children;
};

// Composite Nodes
class BTSequence : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
    void Reset() override;
    
private:
    int currentChildIndex = 0;
};

class BTSelector : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
    void Reset() override;
    
private:
    int currentChildIndex = 0;
};

class BTParallel : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
    void Reset() override;
    
    enum Policy {
        RequireOne,
        RequireAll,
        RequireMajority
    } successPolicy = RequireOne;
    
private:
    std::vector<BTNodeStatus> childStatuses;
};

// Decorator Nodes
class BTInverter : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
};

class BTSucceeder : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
};

class BTFailer : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
};

class BTRepeat : public BTNode {
public:
    BTNodeStatus Execute(float deltaTime) override;
    void Reset() override;
    
    int maxRepeats = -1; // -1 = infinite
    
private:
    int currentRepeat = 0;
};

// Condition Node
using BTConditionFunc = std::function<bool()>;

class BTCondition : public BTNode {
public:
    BTCondition(BTConditionFunc func) : conditionFunc(func) {}
    BTNodeStatus Execute(float deltaTime) override;
    
private:
    BTConditionFunc conditionFunc;
};

// Action Node
using BTActionFunc = std::function<BTNodeStatus(float)>;

class BTAction : public BTNode {
public:
    BTAction(BTActionFunc func) : actionFunc(func) {}
    BTNodeStatus Execute(float deltaTime) override;
    
private:
    BTActionFunc actionFunc;
};

// Blackboard for data sharing
class Blackboard {
public:
    void SetInt(const std::string& key, int value);
    void SetFloat(const std::string& key, float value);
    void SetVector3(const std::string& key, const Vector3& value);
    void SetEntity(const std::string& key, uint32_t value);
    
    int GetInt(const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
    Vector3 GetVector3(const std::string& key, const Vector3& defaultValue = Vector3()) const;
    uint32_t GetEntity(const std::string& key, uint32_t defaultValue = 0) const;
    
    bool HasKey(const std::string& key) const;
    void Clear();
    
private:
    std::unordered_map<std::string, int> intValues;
    std::unordered_map<std::string, float> floatValues;
    std::unordered_map<std::string, Vector3> vectorValues;
    std::unordered_map<std::string, uint32_t> entityValues;
};

// Behavior Tree
class BehaviorTree {
public:
    void SetRoot(std::unique_ptr<BTNode> root);
    BTNodeStatus Update(float deltaTime);
    void Reset();
    
    Blackboard& GetBlackboard() { return blackboard; }
    
private:
    std::unique_ptr<BTNode> root;
    Blackboard blackboard;
};

// NavMesh Polygon
struct NavPolygon {
    std::vector<Vector3> vertices;
    Vector3 normal;
    int neighborIndices[3] = {-1, -1, -1};
    float costs[3] = {1.0f, 1.0f, 1.0f};
    bool isWalkable = true;
};

// NavMesh Agent
struct NavAgent {
    Vector3 position;
    Vector3 velocity;
    float speed = 3.0f;
    float acceleration = 10.0f;
    float stoppingDistance = 0.5f;
    float radius = 0.3f;
    float height = 1.8f;
    
    std::vector<Vector3> currentPath;
    int currentPathIndex = 0;
    
    bool isMoving = false;
    bool pathValid = false;
};

// Navigation Mesh System
class NavMeshSystem {
public:
    static NavMeshSystem& Instance();
    
    // Build navmesh from scene geometry
    void BuildNavMesh(const std::vector<Vector3>& vertices, const std::vector<uint32_t>& indices);
    void Clear();
    
    // Pathfinding
    std::vector<Vector3> FindPath(const Vector3& start, const Vector3& end);
    bool IsValidPosition(const Vector3& position) const;
    Vector3 ProjectToNavMesh(const Vector3& position) const;
    
    // Agents
    uint32_t CreateAgent(const NavAgent& agent);
    void DestroyAgent(uint32_t agentID);
    NavAgent* GetAgent(uint32_t agentID);
    
    void SetAgentTarget(uint32_t agentID, const Vector3& target);
    void StopAgent(uint32_t agentID);
    
    void UpdateAgents(float deltaTime);
    
    // Debug
    const std::vector<NavPolygon>& GetPolygons() const { return polygons; }
    
private:
    NavMeshSystem();
    ~NavMeshSystem();
    
    std::vector<NavPolygon> polygons;
    std::unordered_map<uint32_t, NavAgent> agents;
    uint32_t nextAgentID = 1;
    
    // A* implementation
    struct AStarNode {
        Vector3 position;
        int polygonIndex;
        float gCost; // Cost from start
        float hCost; // Heuristic cost to end
        int parentIndex;
        
        float FCost() const { return gCost + hCost; }
    };
    
    std::vector<AStarNode> FindPathAStar(const Vector3& start, const Vector3& end);
    float CalculateHeuristic(const Vector3& a, const Vector3& b) const;
};

// Crowd Simulation
struct CrowdAgent {
    Vector3 position;
    Vector3 velocity;
    Vector3 desiredVelocity;
    float mass = 1.0f;
    float maxSpeed = 2.0f;
    float maxForce = 10.0f;
    float radius = 0.3f;
    
    std::vector<Vector3> neighbors;
};

class CrowdSimulation {
public:
    uint32_t AddAgent(const CrowdAgent& agent);
    void RemoveAgent(uint32_t id);
    
    void SetGoal(uint32_t agentID, const Vector3& goal);
    void AddObstacle(const Vector3& position, float radius);
    
    void Update(float deltaTime);
    
    const std::vector<CrowdAgent>& GetAgents() const { return agents; }
    
private:
    std::vector<CrowdAgent> agents;
    std::vector<std::pair<Vector3, float>> obstacles;
    
    // Steering behaviors
    Vector3 Seek(CrowdAgent& agent, const Vector3& target);
    Vector3 Avoid(CrowdAgent& agent);
    Vector3 Separate(const CrowdAgent& agent);
    Vector3 Align(const CrowdAgent& agent);
    Vector3 Cohere(const CrowdAgent& agent);
};

} // namespace Omega
