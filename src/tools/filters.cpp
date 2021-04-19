#include "Arduino.h"
#include "tools/filters.h"



// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(uint16_t sample, float filter_factor) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample;
    yv[0] = yv[1]; 
    yv[1] = ((xv[0] + xv[1]) / 2.f + (filter_factor * yv[0])) / (1 + filter_factor);
    return yv[1];
}

void filters_scale(uint16_t &input, float &max_value, uint16_t &limit) {
    if(input > max_value)   // store max value if exceeded
        max_value = input;

    input = (input / max_value) * 1023; // map to 0 - 1023

    if(max_value > limit)   // decrease max_value if it exceeds limit
        max_value -= float(FILTERS_SCALE_MAX_VALUE_DECREASE / FILTERS_FREQ_MAIN_LOOP_HZ);
}
