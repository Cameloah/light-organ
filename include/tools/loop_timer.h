#include "Arduino.h"

// overall loop freq. of the main loop, use this in all timedependent modules
#define FREQ_LOOP_CYCLE_HZ            100

#define FREQ_REPORT_UPPER_LIMIT       1.1       // equals  to 110%

extern uint32_t loop_timer;
extern uint64_t t_0;

/// \brief calculates loop frequency, only correct when 
///        loop_timer is updated every loop iteration
/// \return loop frequency
///
float loop_timer_get_loop_freq();

/// \brief measures loop cycle time and adds delay 
///        if neccessary to meet predefined FREQ_LOOP_CYCLE_HZ
///
void loop_timer_check_cycle_freq();