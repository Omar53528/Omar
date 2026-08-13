#pragma once

#include "core/Types.h"
#include <vector>
#include <string>

namespace Omega {

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
};

struct Mesh {
    std::string path;
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;
    uint32_t indexCount = 0;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    Mesh() = default;
};

} // namespace Omega
