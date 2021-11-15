#include "tools/loop_timer.h"


uint32_t loop_timer         = 0;
double loop_time            = 0;
double loop_timer_start     = 0;
uint64_t t_0                = 0;
uint64_t t_end              = 0;


float loop_timer_get_loop_freq() {
    loop_time = millis() - loop_timer_start;
    loop_time = loop_time / loop_timer;
    loop_timer = 0;
    loop_timer_start = millis();
    return 1000.f/loop_time;
}

void loop_timer_check_cycle_freq() {
    t_end = micros();
    uint64_t t_delta = t_end - t_0;
    if(t_delta < (1000000 / FREQ_LOOP_CYCLE_HZ)) {
        delayMicroseconds((1000000 / FREQ_LOOP_CYCLE_HZ) - t_delta);
    }
}