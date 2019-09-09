// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

// A simple program that computes the square root of a number
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/Timer.h"

int main(int argc, char *argv[])
{
    PERFSTUBS_INIT();
    PERFSTUBS_REGISTER_THREAD()
    PERFSTUBS_METADATA("meta", "data")
    PERFSTUBS_TIMER_START_FUNC(timer);

    // starts
    PERFSTUBS_TIMER_START(timer2, "timer")

    for (int i = 0 ; i < 5; i++) {
        PERFSTUBS_DYNAMIC_PHASE_START("iter", i)
        PERFSTUBS_DYNAMIC_PHASE_STOP("iter", i)
    }

    // stops
    PERFSTUBS_TIMER_STOP(timer2)

    PERFSTUBS_SAMPLE_COUNTER("counter", 15.0)
    PERFSTUBS_TIMER_STOP_FUNC(timer);
    PERFSTUBS_DUMP_DATA();

    return 0;
}