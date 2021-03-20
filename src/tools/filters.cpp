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