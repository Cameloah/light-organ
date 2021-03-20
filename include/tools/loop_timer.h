#include "Arduino.h"

extern uint32_t loop_timer;

//// /brief calculates loop frequency correctly when 
////        loop_timer is updated every loop iteration
//// /return loop frequency
////
float get_loop_freq();