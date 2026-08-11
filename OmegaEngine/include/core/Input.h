#pragma once

#include "core/Types.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>

namespace Omega {

class Input {
public:
    static void init();
    static void shutdown();
    static void update();
    
    // لوحة المفاتيح
    static bool isKeyDown(int keycode);
    static bool isKeyPressed(int keycode);
    static bool isKeyReleased(int keycode);
    
    // الماوس
    static Vector2 getMousePosition();
    static float getMouseX();
    static float getMouseY();
    static bool isMouseButtonDown(int button);
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonReleased(int button);
    
    // Gamepad
    static bool isGamepadConnected(int id = 0);
    static float getGamepadAxis(int id, int axis);
    static bool isGamepadButtonDown(int id, int button);
    
    // Events
    using KeyEventCallback = std::function<void(int, bool)>;
    using MouseEventCallback = std::function<void(int, int, bool)>;
    
    static void setKeyEventCallback(KeyEventCallback callback);
    static void setMouseEventCallback(MouseEventCallback callback);
    
private:
    static std::unordered_map<int, bool> s_currentKeys;
    static std::unordered_map<int, bool> s_previousKeys;
    static std::unordered_map<int, bool> s_currentMouseButtons;
    static std::unordered_map<int, bool> s_previousMouseButtons;
    static int s_mouseX;
    static int s_mouseY;
    static KeyEventCallback s_keyCallback;
    static MouseEventCallback s_mouseCallback;
};

// رموز المفاتيح الشائعة
namespace Keys {
    constexpr int A = SDLK_a;
    constexpr int B = SDLK_b;
    constexpr int C = SDLK_c;
    constexpr int D = SDLK_d;
    constexpr int E = SDLK_e;
    constexpr int F = SDLK_f;
    constexpr int G = SDLK_g;
    constexpr int H = SDLK_h;
    constexpr int I = SDLK_i;
    constexpr int J = SDLK_j;
    constexpr int K = SDLK_k;
    constexpr int L = SDLK_l;
    constexpr int M = SDLK_m;
    constexpr int N = SDLK_n;
    constexpr int O = SDLK_o;
    constexpr int P = SDLK_p;
    constexpr int Q = SDLK_q;
    constexpr int R = SDLK_r;
    constexpr int S = SDLK_s;
    constexpr int T = SDLK_t;
    constexpr int U = SDLK_u;
    constexpr int V = SDLK_v;
    constexpr int W = SDLK_w;
    constexpr int X = SDLK_x;
    constexpr int Y = SDLK_y;
    constexpr int Z = SDLK_z;
    
    constexpr int Space = SDLK_SPACE;
    constexpr int Enter = SDLK_RETURN;
    constexpr int Escape = SDLK_ESCAPE;
    constexpr int Shift = SDLK_LSHIFT;
    constexpr int Control = SDLK_LCTRL;
    constexpr int Alt = SDLK_LALT;
    
    constexpr int Up = SDLK_UP;
    constexpr int Down = SDLK_DOWN;
    constexpr int Left = SDLK_LEFT;
    constexpr int Right = SDLK_RIGHT;
}

namespace Mouse {
    constexpr int Left = SDL_BUTTON_LEFT;
    constexpr int Right = SDL_BUTTON_RIGHT;
    constexpr int Middle = SDL_BUTTON_MIDDLE;
}

} // namespace Omega
