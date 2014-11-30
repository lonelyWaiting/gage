#pragma once

class Clock
{
public:
    static void Startup();
    static void Shutdown();

    // Call Tick once a frame to tick game clock
    static void Tick();

    // The absolute time since startup
    static float GetAbsTimeSec();

    // The time delta since the last frame
    static float GetDeltaTimeSec();

    // The current time value
    typedef unsigned __int64 Timer;
    static Timer GetTimer();

    // The time delta between the input timer and now
    static float GetTimeDeltaSec(Timer start);
};
