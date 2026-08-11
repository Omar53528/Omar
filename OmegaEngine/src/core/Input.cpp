#include "core/Input.h"
#include "core/Log.h"

namespace Omega {

std::unordered_map<int, bool> Input::s_currentKeys;
std::unordered_map<int, bool> Input::s_previousKeys;
std::unordered_map<int, bool> Input::s_currentMouseButtons;
std::unordered_map<int, bool> Input::s_previousMouseButtons;
int Input::s_mouseX = 0;
int Input::s_mouseY = 0;
Input::KeyEventCallback Input::s_keyCallback = nullptr;
Input::MouseEventCallback Input::s_mouseCallback = nullptr;

void Input::init() {
    Log::info("Input System initialized");
}

void Input::shutdown() {
    s_currentKeys.clear();
    s_previousKeys.clear();
    s_currentMouseButtons.clear();
    s_previousMouseButtons.clear();
}

void Input::update() {
    // نقل الحالة الحالية إلى السابقة
    s_previousKeys = s_currentKeys;
    s_previousMouseButtons = s_currentMouseButtons;
    
    // مسح الحالة الحالية
    s_currentKeys.clear();
    s_currentMouseButtons.clear();
    
    // قراءة حالة المفاتيح والماوس الحالية
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    int numKeys;
    const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);
    
    for (int i = 0; i < numKeys; i++) {
        if (keyboardState[i]) {
            s_currentKeys[i] = true;
        }
    }
    
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    s_mouseX = mouseX;
    s_mouseY = mouseY;
    
    for (int i = 1; i <= 3; i++) { // أزرار الماوس الثلاثة الرئيسية
        if (mouseState & SDL_BUTTON(i)) {
            s_currentMouseButtons[i] = true;
        }
    }
    
    // استدعاء الـ callbacks
    if (s_keyCallback) {
        for (const auto& [key, pressed] : s_currentKeys) {
            if (s_previousKeys.find(key) == s_previousKeys.end()) {
                s_keyCallback(key, true);
            }
        }
    }
    
    if (s_mouseCallback) {
        for (const auto& [button, pressed] : s_currentMouseButtons) {
            if (s_previousMouseButtons.find(button) == s_previousMouseButtons.end()) {
                s_mouseCallback(button, s_mouseX, s_mouseY);
            }
        }
    }
}

bool Input::isKeyDown(int keycode) {
    return s_currentKeys.count(keycode) > 0;
}

bool Input::isKeyPressed(int keycode) {
    return s_currentKeys.count(keycode) > 0 && s_previousKeys.count(keycode) == 0;
}

bool Input::isKeyReleased(int keycode) {
    return s_currentKeys.count(keycode) == 0 && s_previousKeys.count(keycode) > 0;
}

Vector2 Input::getMousePosition() {
    return Vector2(static_cast<float>(s_mouseX), static_cast<float>(s_mouseY));
}

float Input::getMouseX() {
    return static_cast<float>(s_mouseX);
}

float Input::getMouseY() {
    return static_cast<float>(s_mouseY);
}

bool Input::isMouseButtonDown(int button) {
    return s_currentMouseButtons.count(button) > 0;
}

bool Input::isMouseButtonPressed(int button) {
    return s_currentMouseButtons.count(button) > 0 && s_previousMouseButtons.count(button) == 0;
}

bool Input::isMouseButtonReleased(int button) {
    return s_currentMouseButtons.count(button) == 0 && s_previousMouseButtons.count(button) > 0;
}

bool Input::isGamepadConnected(int id) {
    return SDL_GameControllerGetAttached(SDL_GameControllerOpen(id)) == SDL_TRUE;
}

float Input::getGamepadAxis(int id, int axis) {
    SDL_GameController* controller = SDL_GameControllerOpen(id);
    if (!controller) return 0.0f;
    
    Sint16 value = SDL_GameControllerGetAxis(controller, static_cast<SDL_GameControllerAxis>(axis));
    return value / 32767.0f;
}

bool Input::isGamepadButtonDown(int id, int button) {
    SDL_GameController* controller = SDL_GameControllerOpen(id);
    if (!controller) return false;
    
    return SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(button)) == 1;
}

void Input::setKeyEventCallback(KeyEventCallback callback) {
    s_keyCallback = callback;
}

void Input::setMouseEventCallback(MouseEventCallback callback) {
    s_mouseCallback = callback;
}

} // namespace Omega
