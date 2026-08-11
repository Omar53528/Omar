#include "graphics/Material.h"
#include <cmath>
#include <random>

namespace Omega {

// Procedural Texture Generation
std::vector<Color> ProceduralTexture::GenerateCheckerboard(int width, int height, const Color& c1, const Color& c2, int tileSize) {
    std::vector<Color> pixels(width * height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int tileX = (x / tileSize) % 2;
            int tileY = (y / tileSize) % 2;
            bool isEven = (tileX + tileY) % 2 == 0;
            pixels[y * width + x] = isEven ? c1 : c2;
        }
    }
    
    return pixels;
}

std::vector<Color> ProceduralTexture::GenerateNoise(int width, int height, float scale) {
    std::vector<Color> pixels(width * height);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Simple value noise
    std::vector<float> noiseMap(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            noiseMap[y * width + x] = dist(gen);
        }
    }
    
    // Simple smoothing
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sum = 0.0f;
            int count = 0;
            
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        sum += noiseMap[ny * width + nx];
                        count++;
                    }
                }
            }
            
            float value = sum / count;
            pixels[y * width + x] = Color(value, value, value, 1.0f);
        }
    }
    
    return pixels;
}

std::vector<Color> ProceduralTexture::GenerateGradient(int width, int height, const Color& start, const Color& end, bool vertical) {
    std::vector<Color> pixels(width * height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float t = vertical ? (float)y / height : (float)x / width;
            pixels[y * width + x] = Color(
                start.r + (end.r - start.r) * t,
                start.g + (end.g - start.g) * t,
                start.b + (end.b - start.b) * t,
                start.a + (end.a - start.a) * t
            );
        }
    }
    
    return pixels;
}

std::vector<Color> ProceduralTexture::GenerateNormalMap(int width, int height, float strength) {
    std::vector<Color> normals(width * height);
    
    // Generate a simple bump pattern
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = (float)x / width;
            float v = (float)y / height;
            
            // Simple sine wave pattern
            float dx = std::sin(u * 10.0f * 3.14159f) * strength;
            float dy = std::sin(v * 10.0f * 3.14159f) * strength;
            
            Vector3 normal(dx, dy, 1.0f);
            normal = normal.Normalized();
            
            // Convert from [-1,1] to [0,1]
            normals[y * width + x] = Color(
                normal.x * 0.5f + 0.5f,
                normal.y * 0.5f + 0.5f,
                normal.z * 0.5f + 0.5f,
                1.0f
            );
        }
    }
    
    return normals;
}

// Material Manager Implementation
MaterialManager& MaterialManager::Instance() {
    static MaterialManager instance;
    return instance;
}

MaterialManager::MaterialManager() {
    // Create default material
    PBRMaterial defaultMat;
    defaultMat.name = "Default";
    defaultMaterialID = CreateMaterial(defaultMat);
    
    // Create metal material
    PBRMaterial metalMat;
    metalMat.name = "Metal";
    metalMat.metallic = 0.9f;
    metalMat.roughness = 0.2f;
    metalMat.albedo = Color(0.8f, 0.8f, 0.85f, 1.0f);
    metalMaterialID = CreateMaterial(metalMat);
    
    // Create glass material
    PBRMaterial glassMat;
    glassMat.name = "Glass";
    glassMat.metallic = 0.0f;
    glassMat.roughness = 0.05f;
    glassMat.albedo = Color(0.9f, 0.95f, 1.0f, 0.3f);
    glassMat.alphaThreshold = 0.1f;
    glassMat.alphaClip = true;
    glassMaterialID = CreateMaterial(glassMat);
}

MaterialManager::~MaterialManager() {}

uint32_t MaterialManager::CreateMaterial(const PBRMaterial& material) {
    uint32_t id = nextID++;
    materials[id] = material;
    return id;
}

void MaterialManager::UpdateMaterial(uint32_t id, const PBRMaterial& material) {
    auto it = materials.find(id);
    if (it != materials.end()) {
        it->second = material;
    }
}

PBRMaterial* MaterialManager::GetMaterial(uint32_t id) {
    auto it = materials.find(id);
    return it != materials.end() ? &it->second : nullptr;
}

void MaterialManager::DestroyMaterial(uint32_t id) {
    materials.erase(id);
}

// Visual Shader Implementation
uint32_t VisualShader::AddNode(ShaderNodeType type, const std::string& name, const Vector2& pos) {
    ShaderNode node;
    node.id = nextNodeID++;
    node.type = type;
    node.name = name;
    node.position = pos;
    nodes.push_back(node);
    return node.id;
}

void VisualShader::RemoveNode(uint32_t nodeID) {
    nodes.erase(
        std::remove_if(nodes.begin(), nodes.end(),
            [nodeID](const ShaderNode& n) { return n.id == nodeID; }),
        nodes.end()
    );
}

void VisualShader::ConnectNodes(uint32_t fromNode, int fromOutput, uint32_t toNode, int toInput) {
    for (auto& node : nodes) {
        if (node.id == toNode) {
            node.inputs.push_back({fromNode, fromOutput});
        }
        if (node.id == fromNode) {
            node.outputs.push_back(fromOutput);
        }
    }
}

void VisualShader::DisconnectNodes(uint32_t fromNode, int fromOutput, uint32_t toNode, int toInput) {
    for (auto& node : nodes) {
        if (node.id == toNode) {
            node.inputs.erase(
                std::remove_if(node.inputs.begin(), node.inputs.end(),
                    [fromNode, fromOutput](const auto& conn) {
                        return conn.first == fromNode && conn.second == fromOutput;
                    }),
                node.inputs.end()
            );
        }
        if (node.id == fromNode) {
            node.outputs.erase(
                std::remove(node.outputs.begin(), node.outputs.end(), fromOutput),
                node.outputs.end()
            );
        }
    }
}

std::string VisualShader::CompileToGLSL(bool isVertex) {
    std::string code;
    
    // Find output nodes
    for (const auto& node : nodes) {
        if (node.type == ShaderNodeType::Output_Albedo && !isVertex) {
            code += "// Albedo Output\n";
            // Generate code for connected nodes
        }
    }
    
    return code;
}

std::string VisualShader::GenerateNodeCode(const ShaderNode& node, bool isVertex) {
    switch (node.type) {
        case ShaderNodeType::Constant_Float:
            return std::to_string(node.floatValue);
        case ShaderNodeType::Constant_Vector3:
            return "vec3(" + std::to_string(node.vectorValue.x) + ", " + 
                   std::to_string(node.vectorValue.y) + ", " + 
                   std::to_string(node.vectorValue.z) + ")";
        case ShaderNodeType::Constant_Color:
            return "vec4(" + std::to_string(node.colorValue.r) + ", " + 
                   std::to_string(node.colorValue.g) + ", " + 
                   std::to_string(node.colorValue.b) + ", " + 
                   std::to_string(node.colorValue.a) + ")";
        default:
            return "";
    }
}

} // namespace Omega
