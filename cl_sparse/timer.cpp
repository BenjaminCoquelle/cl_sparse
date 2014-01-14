#include "timer.h"
#include <sys/time.h>

Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    struct timeval t;
    struct timezone tz;

    gettimeofday(&t, &tz);
    base_sec = t.tv_sec;
    base_usec = t.tv_usec;
}

double Timer::elapsed_seconds()
{
    const double mega = 0.000001;

    struct timeval t;
    struct timezone tz;
    gettimeofday(&t, &tz);

    double time = (double) (t.tv_sec - base_sec);
    double mic = (double) (t.tv_usec - base_usec);
    return time + mic * mega;
}

