#ifndef TIMER_H
#define TIMER_H

#include <string>

class Timer
{
public:
    //trigger the counting during creation time
    Timer();

    //reset timer start to measure again
    void reset();

    //get number of elapsed seconds
    double elapsed_seconds();

private:

    long int base_sec;
    long int base_usec;

};

#endif // TIMER_H
