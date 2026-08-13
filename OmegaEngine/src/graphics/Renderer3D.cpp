#include "graphics/Renderer3D.h"

namespace Omega {

Renderer3D::SceneData Renderer3D::s_SceneData;
bool Renderer3D::s_Initialized = false;

void Renderer3D::Init() {
    if (s_Initialized) return;
    s_Initialized = true;
}

void Renderer3D::Shutdown() {
    s_Initialized = false;
}

void Renderer3D::BeginScene(const Camera3D& camera) {
    s_SceneData.viewMatrix = camera.GetViewMatrix();
    s_SceneData.projectionMatrix = camera.GetProjectionMatrix();
    s_SceneData.viewProjection = s_SceneData.projectionMatrix * s_SceneData.viewMatrix;
}

void Renderer3D::EndScene() {
    // Flush batch
}

void Renderer3D::DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Material& material) {
    if (!s_Initialized) return;
    // OpenGL implementation would go here
}

} // namespace Omega
