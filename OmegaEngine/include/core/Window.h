#pragma once

#include "core/Types.h"
#include <SDL2/SDL.h>

namespace Omega {

class Window {
public:
    Window();
    ~Window();
    
    bool create(const std::string& title, int width, int height, bool fullscreen = false);
    void destroy();
    
    bool shouldClose() const;
    void pollEvents();
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    SDL_Window* getSDLWindow() { return m_window; }
    SDL_GLContext getGLContext() { return m_glContext; }
    
    void swapBuffers();
    void setVSync(bool enabled);
    
private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    int m_width = 0;
    int m_height = 0;
    bool m_shouldClose = false;
};

} // namespace Omega
