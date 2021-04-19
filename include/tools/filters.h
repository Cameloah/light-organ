#include "Arduino.h"
#include "tools/loop_timer.h"

// import main loop freq
#define FILTERS_FREQ_MAIN_LOOP_HZ               FREQ_LOOP_CYCLE_HZ

// how how much the stored max value is decreasing per second
#define FILTERS_SCALE_MAX_VALUE_DECREASE        40

/// \brief WIP
///
float filters_envelopeFilter(uint16_t sample, float filter_factor);

/// \brief scales an input to 0 - 1023 and stores the max reported value in max_value.
///        when the max value exceeds the limit, the stored max value will be decreased over time back to the limit
///
/// \param input reference to new input value
/// \param max_value reference to data buffer where the current max value is stored
/// \param limit reference to the limit, the max value drops to, when no higher value is detected
///
void filters_scale(uint16_t *input, float &max_value, uint16_t limit);