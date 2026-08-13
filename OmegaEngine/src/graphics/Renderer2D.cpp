#include "graphics/Renderer2D.h"
#include <iostream>

namespace Omega {

Renderer2D::SceneData Renderer2D::s_SceneData;
bool Renderer2D::s_Initialized = false;

void Renderer2D::Init() {
    if (s_Initialized) return;
    
    std::cout << "[Renderer2D] Initialized successfully" << std::endl;
    s_Initialized = true;
}

void Renderer2D::Shutdown() {
    if (!s_Initialized) return;
    
    std::cout << "[Renderer2D] Shutdown" << std::endl;
    s_Initialized = false;
}

void Renderer2D::BeginScene(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix) {
    s_SceneData.viewProjection = projectionMatrix * viewMatrix;
}

void Renderer2D::EndScene() {
    // Flush batch
}

void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, const Color& color) {
    if (!s_Initialized) return;
    // Implementation would use OpenGL to draw a colored quad
}

void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, Texture2D* texture, float tilingFactor) {
    if (!s_Initialized) return;
    // Implementation would use OpenGL to draw a textured quad
}

void Renderer2D::DrawSprite(const Vector2& position, Texture2D* texture, const Vector2& size, int frame) {
    DrawQuad(position, size, texture);
}

void Renderer2D::DrawLine(const Vector2& start, const Vector2& end, const Color& color, float thickness) {
    if (!s_Initialized) return;
    // Implementation would use OpenGL line drawing
}

void Renderer2D::DrawCircle(const Vector2& center, float radius, const Color& color, int segments) {
    if (!s_Initialized) return;
    // Implementation would use OpenGL to draw a circle
}

void Renderer2D::DrawText(const std::string& text, const Vector2& position, float size, const Color& color) {
    if (!s_Initialized) return;
    // Implementation would render text using a font atlas
}

} // namespace Omega
