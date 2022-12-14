#pragma once

#include <debugapi.h>
#include <processthreadsapi.h>
#include <profileapi.h>
#include <stdint.h>
#include <stdlib.h>

namespace NEngine {
namespace Utils {
#define MAX_DELTA_TIME 1000

struct Timer
{
    Timer()
        : DeltaMillis(0.0f),
          StartTime(),
          EndTime(),
          Frequency(),
          ElapsedMicroseconds()
    {
    }

    inline auto
    Initialize() -> void
    {
        QueryPerformanceFrequency(&Frequency);
    }

    inline auto
    Update() -> void
    {
        if (!QueryPerformanceCounter(&EndTime)) {
            OutputDebugStringA("ERROR: Failed to query performance counter\n");
            ExitProcess(EXIT_FAILURE);
        }

        uint64_t timeDelta = EndTime.QuadPart - StartTime.QuadPart;

        timeDelta *= 1000000;
        timeDelta /= Frequency.QuadPart;

        DeltaMillis = (double)timeDelta / 1000.0;  // convert to ms
        if (DeltaMillis > MAX_DELTA_TIME) {
            DeltaMillis = MAX_DELTA_TIME;
        }
        StartTime = EndTime;
    }

    inline auto
    GetDelta() -> float
    {
        return static_cast<float>(DeltaMillis);
    }

    double DeltaMillis;
    LARGE_INTEGER StartTime;
    LARGE_INTEGER EndTime;
    LARGE_INTEGER Frequency;
    LARGE_INTEGER ElapsedMicroseconds;
};

inline void
TimerInitialize(Timer *timer)
{
    timer->DeltaMillis = 0.0;
    memset(timer, 0, sizeof(Timer));
    QueryPerformanceFrequency(&timer->Frequency);
}

inline void
TimerTick(Timer *timer)
{
    if (!QueryPerformanceCounter(&timer->EndTime)) {
        OutputDebugStringA("ERROR: Failed to query performance counter\n");
        ExitProcess(EXIT_FAILURE);
    }

    uint64_t timeDelta = timer->EndTime.QuadPart - timer->StartTime.QuadPart;

    timeDelta *= 1000000;
    timeDelta /= timer->Frequency.QuadPart;

    timer->DeltaMillis = (double)timeDelta / 1000.0;  // convert to ms
    if (timer->DeltaMillis > MAX_DELTA_TIME)
        timer->DeltaMillis = MAX_DELTA_TIME;
    timer->StartTime = timer->EndTime;
}

}  // namespace Utils
}  // namespace NEngine