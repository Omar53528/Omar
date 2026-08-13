#pragma once

#include <string>

namespace Omega {

class Shader {
public:
    std::string vertexPath;
    std::string fragmentPath;
    uint32_t id = 0;
    
    Shader() = default;
    Shader(const std::string& vPath, const std::string& fPath) 
        : vertexPath(vPath), fragmentPath(fPath) {}
};

} // namespace Omega
