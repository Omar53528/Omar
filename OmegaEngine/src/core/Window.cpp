#include "core/Window.h"
#include "core/Log.h"
#include <SDL2/SDL_opengl.h>

namespace Omega {

Window::Window() = default;

Window::~Window() {
    destroy();
}

bool Window::create(const std::string& title, int width, int height, bool fullscreen) {
    if (m_window) {
        Log::warn("Window already created");
        return false;
    }
    
    // إعداد OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    
    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 width, height, flags);
    
    if (!m_window) {
        Log::error("Failed to create window: " + std::string(SDL_GetError()));
        return false;
    }
    
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        Log::error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return false;
    }
    
    SDL_GL_MakeCurrent(m_window, m_glContext);
    
    // التحقق من دعم OpenGL
    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    Log::info("OpenGL Version: " + std::to_string(major) + "." + std::to_string(minor));
    
    m_width = width;
    m_height = height;
    m_shouldClose = false;
    
    Log::info("Window created: " + title + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");
    return true;
}

void Window::destroy() {
    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

bool Window::shouldClose() const {
    return m_shouldClose;
}

void Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_shouldClose = true;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                m_width = event.window.data1;
                m_height = event.window.data2;
                glViewport(0, 0, m_width, m_height);
            }
        }
    }
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

void Window::setVSync(bool enabled) {
    SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

} // namespace Omega
