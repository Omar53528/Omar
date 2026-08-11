#pragma once

namespace Omega {

class Time {
public:
    static void update();
    
    static double getDeltaTime();
    static float getDeltaTimeF();
    static double getTotalTime();
    static float getFPS();
    
private:
    static double s_deltaTime;
    static double s_totalTime;
    static float s_fps;
    static auto s_lastFrame;
    static int s_frameCount;
    static auto s_fpsTime;
};

} // namespace Omega
