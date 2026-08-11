#include "core/Time.h"
#include <chrono>

namespace Omega {

double Time::s_deltaTime = 0.0;
double Time::s_totalTime = 0.0;
float Time::s_fps = 0.0f;
auto Time::s_lastFrame = std::chrono::high_resolution_clock::now();
int Time::s_frameCount = 0;
auto Time::s_fpsTime = std::chrono::high_resolution_clock::now();

void Time::update() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> delta = now - s_lastFrame;
    s_deltaTime = delta.count();
    s_totalTime += s_deltaTime;
    s_lastFrame = now;
    
    s_frameCount++;
    
    auto fpsNow = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> fpsDelta = fpsNow - s_fpsTime;
    
    if (fpsDelta.count() >= 1.0) {
        s_fps = static_cast<float>(s_frameCount) / static_cast<float>(fpsDelta.count());
        s_frameCount = 0;
        s_fpsTime = fpsNow;
    }
}

double Time::getDeltaTime() { return s_deltaTime; }
float Time::getDeltaTimeF() { return static_cast<float>(s_deltaTime); }
double Time::getTotalTime() { return s_totalTime; }
float Time::getFPS() { return s_fps; }

} // namespace Omega
