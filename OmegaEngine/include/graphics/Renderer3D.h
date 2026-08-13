#pragma once

#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "graphics/Camera3D.h"

namespace Omega {

class Renderer3D {
public:
    static void Init();
    static void Shutdown();
    
    static void BeginScene(const Camera3D& camera);
    static void EndScene();
    
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Material& material);
    
private:
    struct SceneData {
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        Matrix4x4 viewProjection;
    };
    static SceneData s_SceneData;
    static bool s_Initialized;
};

} // namespace Omega
