#pragma once

#include "core/Types.h"
#include <vector>
#include <functional>

namespace Omega {

// Terrain Chunk Data
struct TerrainChunk {
    int x, z; // Chunk coordinates
    int resolution = 64;
    std::vector<float> heights;
    std::vector<Vector3> normals;
    std::vector<Color> splatMap; // For texture blending
    
    bool isDirty = true; // Needs regeneration
    bool isVisible = false;
};

// Tree Instance for Instanced Rendering
struct TreeInstance {
    Vector3 position;
    float scale = 1.0f;
    float rotation = 0.0f;
    uint32_t treeType = 0; // Index to tree model
    Color tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
};

// Grass Blade (GPU instanced)
struct GrassInstance {
    Vector2 position;
    float height = 0.3f;
    float rotation = 0.0f;
    float width = 0.05f;
    Color color = Color(0.2f, 0.6f, 0.1f, 1.0f);
};

// Sky Settings
struct SkySettings {
    enum SkyMode {
        SkyMode_SolidColor,
        SkyMode_Gradient,
        SkyMode_Procedural,
        SkyMode_HDRISkybox
    };
    
    SkyMode mode = SkyMode_Procedural;
    Color topColor = Color(0.4f, 0.6f, 0.9f, 1.0f);
    Color horizonColor = Color(0.8f, 0.9f, 1.0f, 1.0f);
    Color groundColor = Color(0.3f, 0.25f, 0.2f, 1.0f);
    
    // Procedural sky
    float sunIntensity = 1.0f;
    Vector3 sunDirection = Vector3(0.5f, 0.5f, 0.0f).Normalized();
    float atmosphericThickness = 1.0f;
    float rayleighStrength = 1.0f;
    float mieStrength = 1.0f;
    
    // HDRI
    uint32_t hdriTextureID = 0;
    float exposure = 1.0f;
    float rotation = 0.0f;
};

// Fog Settings
struct FogSettings {
    bool enabled = true;
    enum FogMode {
        FogMode_Linear,
        FogMode_Exponential,
        FogMode_ExponentialSquared
    };
    
    FogMode mode = FogMode_Exponential;
    Color color = Color(0.5f, 0.5f, 0.5f, 1.0f);
    float density = 0.02f;
    float startDistance = 10.0f;
    float endDistance = 100.0f;
};

// Nature System Manager
class NatureSystem {
public:
    static NatureSystem& Instance();
    
    // Terrain
    void SetTerrainSize(float width, float depth);
    void SetTerrainResolution(int resolution);
    float GetHeightAt(float x, float z) const;
    Vector3 GetNormalAt(float x, float z) const;
    
    // Heightmap manipulation
    using HeightFunction = std::function<float(float, float)>;
    void ApplyHeightFunction(HeightFunction func, float minX, float maxX, float minZ, float maxZ);
    void FlattenTerrain(float x, float z, float radius, float height);
    void RaiseTerrain(float x, float z, float radius, float strength);
    void SmoothTerrain(float x, float z, float radius, float strength);
    
    // Vegetation
    uint32_t AddTreeModel(const std::string& modelPath);
    void PlantTree(uint32_t treeType, const Vector3& position, float scale = 1.0f, float rotation = 0.0f);
    void PlantGrass(const Vector2& position, float height = 0.3f, float width = 0.05f, const Color& color = Color(0.2f, 0.6f, 0.1f, 1.0f));
    
    // Bulk operations
    void AutoPlantTrees(float density, uint32_t seed = 0);
    void AutoPlantGrass(float density, uint32_t seed = 0);
    void ClearVegetation();
    
    // Sky and Atmosphere
    void SetSkySettings(const SkySettings& settings);
    const SkySettings& GetSkySettings() const { return skySettings; }
    
    void SetFogSettings(const FogSettings& settings);
    const FogSettings& GetFogSettings() const { return fogSettings; }
    
    // LOD and Culling
    void SetLODDistances(float near, float mid, float far);
    
private:
    NatureSystem();
    ~NatureSystem();
    
    std::vector<TerrainChunk> terrainChunks;
    std::vector<TreeInstance> trees;
    std::vector<GrassInstance> grass;
    std::vector<std::string> treeModels;
    
    SkySettings skySettings;
    FogSettings fogSettings;
    
    float terrainWidth = 1000.0f;
    float terrainDepth = 1000.0f;
    int terrainResolution = 256;
    
    float lodNear = 50.0f;
    float lodMid = 200.0f;
    float lodFar = 500.0f;
};

} // namespace Omega
