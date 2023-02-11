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

    auto
    Initialize() -> void
    {
        QueryPerformanceFrequency(&Frequency);
    }

    auto
    Update() -> void
    {
        if (!QueryPerformanceCounter(&EndTime)) {
            OutputDebugStringA("ERROR: Failed to query performance counter\n");
            ExitProcess(EXIT_FAILURE);
        }

        if (StartTime.QuadPart == 0) {
            LARGE_INTEGER tmp;
            tmp.QuadPart = 16 * 1000 * Frequency.QuadPart / 1000000.0;
            StartTime.QuadPart = EndTime.QuadPart - tmp.QuadPart;
        }

        uint64_t timeDelta = EndTime.QuadPart - StartTime.QuadPart;
        StartTime = EndTime;

        timeDelta *= 1000000;
        timeDelta /= Frequency.QuadPart;

        DeltaMillis = (double)timeDelta / 1000.0;  // convert to ms
        if (DeltaMillis > MAX_DELTA_TIME) {
            DeltaMillis = MAX_DELTA_TIME;
        }
    }

    auto
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

}  // namespace Utils
}  // namespace NEngine