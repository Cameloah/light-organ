#pragma once

#include "Arduino.h"

// overall loop freq. of the main loop, use this in all timedependent modules
#define FREQ_LOOP_CYCLE_HZ 100

#define FREQ_REPORT_UPPER_LIMIT 1.1 // a frame counts as too slow above 110% of its time

/// \brief marks the start of a loop iteration. Call first thing in loop().
///
void loop_timer_start_cycle();

/// \brief waits for the rest of the cycle time so the loop runs at FREQ_LOOP_CYCLE_HZ.
///        Frames that took too long are counted and reported in the log at most every
///        few minutes. Call last thing in loop().
///
void loop_timer_end_cycle();

/// \brief average loop frequency since the last call
/// \return loop frequency in Hz
///
float loop_timer_get_loop_freq();
