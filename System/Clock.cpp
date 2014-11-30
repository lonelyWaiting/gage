#pragma once
#include "Windows.h"
#include "stdio.h"
#include "Core.h"
#include "Clock.h"

static double freq;
static double total_time;
static Clock::Timer this_frame_start;
static float frame_delta;

void Clock::Startup()
{
    Timer qpf;
    QueryPerformanceFrequency((LARGE_INTEGER*) &qpf);
    freq = 1.0 / (double)qpf;
    
    total_time = 0;
	this_frame_start = Clock::GetTimer();
    frame_delta = 0;
}

void Clock::Shutdown()
{
}

void Clock::Tick()
{
    Timer time = Clock::GetTimer();
    frame_delta = float((time - this_frame_start) * freq);
    total_time += frame_delta;
    this_frame_start = time;
}

float Clock::GetAbsTimeSec()
{
    return (float)total_time;
}

float Clock::GetDeltaTimeSec()
{
    return frame_delta;
}

Clock::Timer Clock::GetTimer()
{
    Timer t;
    QueryPerformanceCounter( (LARGE_INTEGER *) &t );
    return t;
}

float Clock::GetTimeDeltaSec(Timer start)
{
    return float((GetTimer() - start) * freq);
}
