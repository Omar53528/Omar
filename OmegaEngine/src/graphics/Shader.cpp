#include "graphics/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Omega {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    loadFromFile(vertexPath, fragmentPath);
}

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource, bool isSourceCode) {
    if (isSourceCode) {
        compileFromSource(vertexSource, fragmentSource);
    } else {
        loadFromFile(vertexSource, fragmentSource);
    }
}

Shader::~Shader() {
    glDeleteProgram(programID);
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream vFile(vertexPath), fFile(fragmentPath);
    if (!vFile.is_open() || !fFile.is_open()) {
        Log::error("Failed to open shader files: {} or {}", vertexPath, fragmentPath);
        return false;
    }
    
    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();
    
    return compileFromSource(vStream.str(), fStream.str());
}

bool Shader::compileFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vSource = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &vSource, nullptr);
    glCompileShader(vertexShader);
    
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        Log::error("Vertex shader compilation failed: {}", infoLog);
        return false;
    }
    
    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fSource = fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &fSource, nullptr);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        Log::error("Fragment shader compilation failed: {}", infoLog);
        return false;
    }
    
    // Link program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        Log::error("Shader program linking failed: {}", infoLog);
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    Log::info("Shader compiled successfully");
    return true;
}

void Shader::use() const {
    glUseProgram(programID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const Vector2& value) const {
    glUniform2f(glGetUniformLocation(programID, name.c_str()), value.x, value.y);
}

void Shader::setVec3(const std::string& name, const Vector3& value) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, const Vector4& value) const {
    glUniform4f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& name, const Matrix4x4& matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, matrix.data());
}

GLuint Shader::getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(programID, name.c_str());
}

} // namespace Omega
