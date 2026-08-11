#pragma once

#include "core/Types.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace Omega {

// PBR Material Properties
struct PBRMaterial {
    Color albedo = Color(1.0f, 1.0f, 1.0f, 1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    Color emissive = Color(0.0f, 0.0f, 0.0f, 1.0f);
    float emissiveStrength = 1.0f;
    
    // Texture IDs
    uint32_t albedoMap = 0;
    uint32_t normalMap = 0;
    uint32_t metallicRoughnessMap = 0;
    uint32_t aoMap = 0;
    uint32_t emissiveMap = 0;
    uint32_t brdfLUT = 0;
    
    // Advanced settings
    bool doubleSided = false;
    bool alphaClip = false;
    float alphaThreshold = 0.5f;
    
    std::string name;
};

// Procedural Texture Generator
class ProceduralTexture {
public:
    static std::vector<Color> GenerateCheckerboard(int width, int height, const Color& c1, const Color& c2, int tileSize);
    static std::vector<Color> GenerateNoise(int width, int height, float scale);
    static std::vector<Color> GenerateGradient(int width, int height, const Color& start, const Color& end, bool vertical);
    static std::vector<Color> GenerateNormalMap(int width, int height, float strength);
};

// Material Manager
class MaterialManager {
public:
    static MaterialManager& Instance();
    
    uint32_t CreateMaterial(const PBRMaterial& material);
    void UpdateMaterial(uint32_t id, const PBRMaterial& material);
    PBRMaterial* GetMaterial(uint32_t id);
    void DestroyMaterial(uint32_t id);
    
    // Built-in materials
    uint32_t GetDefaultMaterial() const { return defaultMaterialID; }
    uint32_t GetMetalMaterial() const { return metalMaterialID; }
    uint32_t GetGlassMaterial() const { return glassMaterialID; }
    
private:
    MaterialManager();
    ~MaterialManager();
    
    std::unordered_map<uint32_t, PBRMaterial> materials;
    uint32_t nextID = 1;
    uint32_t defaultMaterialID;
    uint32_t metalMaterialID;
    uint32_t glassMaterialID;
};

// Shader Node for Visual Shader Editor
enum class ShaderNodeType {
    Input_Position,
    Input_Normal,
    Input_UV,
    Input_Color,
    Constant_Float,
    Constant_Vector3,
    Constant_Color,
    Texture_Sample,
    Math_Add,
    Math_Subtract,
    Math_Multiply,
    Math_Divide,
    Math_Power,
    Math_Dot,
    Math_Cross,
    Math_Normalize,
    Function_Lerp,
    Function_Clamp,
    Function_Saturate,
    Function_Fresnel,
    Output_Albedo,
    Output_Normal,
    Output_Metallic,
    Output_Roughness,
    Output_Emissive
};

struct ShaderNode {
    uint32_t id;
    ShaderNodeType type;
    std::string name;
    Vector2 position; // Position in shader graph
    
    // Inputs and outputs connections
    std::vector<std::pair<uint32_t, int>> inputs; // (nodeID, outputIndex)
    std::vector<int> outputs; // output indices used by other nodes
    
    // Data based on node type
    float floatValue = 0.0f;
    Vector3 vectorValue;
    Color colorValue;
    uint32_t textureID = 0;
    
    std::string customCode;
};

// Visual Shader Graph
class VisualShader {
public:
    std::vector<ShaderNode> nodes;
    std::string name;
    
    uint32_t AddNode(ShaderNodeType type, const std::string& name, const Vector2& pos);
    void RemoveNode(uint32_t nodeID);
    void ConnectNodes(uint32_t fromNode, int fromOutput, uint32_t toNode, int toInput);
    void DisconnectNodes(uint32_t fromNode, int fromOutput, uint32_t toNode, int toInput);
    
    // Compile visual shader to GLSL code
    std::string CompileToGLSL(bool isVertex = false);
    
private:
    uint32_t nextNodeID = 1;
    std::string GenerateNodeCode(const ShaderNode& node, bool isVertex);
};

} // namespace Omega
