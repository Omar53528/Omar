#include "OmegaEngine.h"
#include <iostream>

using namespace Omega;

int main() {
    std::cout << "=== Omega Engine Demo Game ===" << std::endl;
    
    // Initialize engine
    if (!Engine::Init("Omega Demo", 1280, 720)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return 1;
    }
    
    std::cout << "Engine initialized successfully!" << std::endl;
    
    // Load a scene
    Scene* scene = new Scene();
    
    // Create some entities
    auto player = scene->CreateEntity("Player");
    player->AddComponent<TransformComponent>(Vector3(0.0f, 0.0f, 0.0f));
    
    auto camera = scene->CreateEntity("MainCamera");
    camera->AddComponent<TransformComponent>(Vector3(0.0f, 5.0f, 10.0f));
    
    std::cout << "Scene created with " << scene->GetEntityCount() << " entities" << std::endl;
    
    // Main loop
    bool running = true;
    while (running) {
        // Handle events
        Event event;
        while (Window::PollEvent(event)) {
            if (event.type == EventType::Quit) {
                running = false;
            }
        }
        
        // Update
        float dt = Time::GetDeltaTime();
        scene->Update(dt);
        
        // Render
        Renderer2D::BeginScene(Matrix4x4::Identity(), Matrix4x4::Orthographic(-10, 10, -10, 10));
        Renderer2D::DrawQuad(Vector2(0.0f, 0.0f), Vector2(2.0f, 2.0f), Color(1.0f, 0.5f, 0.0f, 1.0f));
        Renderer2D::EndScene();
        
        Window::SwapBuffers();
    }
    
    // Cleanup
    delete scene;
    Engine::Shutdown();
    
    std::cout << "Demo finished successfully!" << std::endl;
    return 0;
}
