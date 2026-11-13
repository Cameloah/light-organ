#include "tools/filters.h"

void filters_scale(uint16_t* input, float& max_value, uint16_t limit) {
    if (static_cast<float>(*input) > max_value) { // store max value if exceeded
        max_value = static_cast<float>(*input);
    }

    if (max_value <= 0) { // nothing heard yet: avoid dividing by zero
        *input = 0;
        return;
    }

    *input = (uint16_t)((static_cast<float>(*input) / max_value) * 1023); // map to 0 - 1023
    if (max_value > static_cast<float>(limit)) { // decrease max_value if it exceeds limit
        max_value -= float(FILTERS_SCALE_MAX_VALUE_DECREASE / FILTERS_FREQ_MAIN_LOOP_HZ);
    }
}
