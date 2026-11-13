#pragma once

#include <Arduino.h>
#include <ProjectUtils.h>


// ---------- DEBUG AND SYSTEM CONTROL ---------- //

// #define DEBUG_DISPLAY_LOOP_FRQ          // use this to output the loop freq in Hz via serial print
#define SYSCTRL_LOOPTIMER               // enable loop frequency control, remember to also set the loop freq in the loop_timer.h

typedef enum {
    MODE_BLACKOUT   = 0,
    MODE_MUSIC      = 1,
    MODE_AMBIENT    = 2,
    MODE_DMX        = 3
} mode_num_t;




void led_mode_switch(uint8_t module_index);
