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




/// \brief switches the LEDs to another mode, blending over from the current one.
/// Safe to call from any task: the switch itself happens at the start of the next LED frame,
/// on the core that draws the LEDs.
void led_request_mode(mode_num_t mode);
