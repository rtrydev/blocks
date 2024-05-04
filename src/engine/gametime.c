#include "gametime.h"

#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#endif

long deltaTimeValue = -1;
long previousTime = -1;

double deltaTime() {
    return deltaTimeValue / 1000.0;
}

void processDeltaTime() {
    struct timespec timeFetch;

#if defined(_WIN32)
    static LARGE_INTEGER ticksPerSec;
    LARGE_INTEGER ticks;

    if (!ticksPerSec.QuadPart) {
        QueryPerformanceFrequency(&ticksPerSec);
        if (!ticksPerSec.QuadPart) {
            errno = ENOTSUP;
            return -1;
        }
    }

    QueryPerformanceCounter(&ticks);

    timeFetch.tv_sec = (long)(ticks.QuadPart / ticksPerSec.QuadPart);
    timeFetch.tv_nsec = (long)(((ticks.QuadPart % ticksPerSec.QuadPart) * 1000 * 1000 * 1000) / ticksPerSec.QuadPart);
#else
    clock_gettime(CLOCK_MONOTONIC, &timeFetch);
#endif

    long currentTime = timeFetch.tv_sec * 1000 * 1000 + timeFetch.tv_nsec / 1000;

    if (previousTime == -1) {
        previousTime = currentTime;
    }

    deltaTimeValue = currentTime - previousTime;
    previousTime = currentTime;
}
