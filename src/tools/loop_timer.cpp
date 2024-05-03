#include "tools/loop_timer.h"
#include "ram_log.h"


uint32_t loop_timer         = 0;
double loop_time            = 0;
double loop_timer_start     = 0;
uint64_t t_0                = 0;
uint64_t t_end              = 0;
bool loop_timer_skip_flag   = false;


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
    if(t_delta < (1000000 / FREQ_LOOP_CYCLE_HZ))
        delayMicroseconds((1000000 / FREQ_LOOP_CYCLE_HZ) - t_delta);

    else if(t_delta > (1000000 / FREQ_LOOP_CYCLE_HZ * FREQ_REPORT_UPPER_LIMIT) && !loop_timer_skip_flag) {
        // reset flag
        loop_timer_skip_flag = false;
        String payload = "Loop took to long! Loop freq: " + String(loop_timer_get_loop_freq());
        ram_log_notify(RAM_LOG_ERROR_SYSTEM, payload.c_str());
    }
        
}