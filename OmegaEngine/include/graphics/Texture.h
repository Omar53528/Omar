#pragma once

#include "core/Types.h"
#include <string>

namespace Omega {

struct Texture2D {
    std::string path;
    uint32_t id = 0;
    int width = 0;
    int height = 0;
    
    Texture2D() = default;
};

struct Texture3D {
    std::string path;
    uint32_t id = 0;
    int width = 0;
    int height = 0;
    int depth = 0;
    
    Texture3D() = default;
};

} // namespace Omega
