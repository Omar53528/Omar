#pragma once

#include "core/Types.h"
#include <string>
#include <vector>

namespace Omega {

class Renderer2D {
public:
    static void Init();
    static void Shutdown();
    
    static void BeginScene(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix);
    static void EndScene();
    
    // Drawing primitives
    static void DrawQuad(const Vector2& position, const Vector2& size, const Color& color);
    static void DrawQuad(const Vector2& position, const Vector2& size, Texture2D* texture, float tilingFactor = 1.0f);
    static void DrawSprite(const Vector2& position, Texture2D* texture, const Vector2& size = Vector2(1.0f, 1.0f), int frame = 0);
    
    // Line drawing
    static void DrawLine(const Vector2& start, const Vector2& end, const Color& color, float thickness = 1.0f);
    static void DrawCircle(const Vector2& center, float radius, const Color& color, int segments = 32);
    
    // Text (basic)
    static void DrawText(const std::string& text, const Vector2& position, float size, const Color& color);
    
private:
    struct SceneData {
        Matrix4x4 viewProjection;
    };
    static SceneData s_SceneData;
    static bool s_Initialized;
};

} // namespace Omega
