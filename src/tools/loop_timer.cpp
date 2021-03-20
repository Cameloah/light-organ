#include "tools/loop_timer.h"


uint32_t loop_timer = 0;
double loop_time = 0;
double timer_start = 0;


void loop_timer_update() {

}

float get_loop_freq() {
    loop_time = millis() - timer_start;
    loop_time = loop_time / loop_timer;
    loop_timer = 0;
    timer_start = millis();
    return 1000.f/loop_time;
}

uint16_t check_cycle_freq() {
    
}