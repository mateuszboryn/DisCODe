#ifndef _QUAD_
#define _QUAD_

#ifdef _WIN32
#include <windows.h>
#else
#include <ctime>
#endif
 
class Timer
{
#ifdef _WIN32
unsigned long start;
#else
timespec start;
timespec stop;
#endif
unsigned long time;
 
public:
void Start()
{
#ifdef _WIN32
start = GetTickCount ();
#else
do
clock_gettime(CLOCK_REALTIME, &start);
while (start.tv_nsec < 0 || start.tv_nsec >= 1000000000L);
#endif
}

unsigned long Stop()
{
#ifdef _WIN32
time = GetTickCount() - start;
return time;
#else
do
clock_gettime(CLOCK_REALTIME, &stop);
while (stop.tv_nsec < 0 || stop.tv_nsec >= 1000000000L);
time = (stop.tv_sec - start.tv_sec)*1000 + (stop.tv_nsec - start.tv_nsec) / 1000000L;
return time;
#endif
}
unsigned long GetTime()
{
return time;
}
};

#endif