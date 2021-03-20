// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

#include <Arduino.h>
#include <tools/spectrum_analyzer.h>
#include "tools/beat_detector.h"



// Our Global Sample Rate, 5000hz
#define SAMPLEPERIODUS 200

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void beat_detector_init() {
    // Set ADC to 77khz, max for 10bit
    sbi(ADCSRA,ADPS2);
    cbi(ADCSRA,ADPS1);
    cbi(ADCSRA,ADPS0);

    //The pin with the LED
    pinMode(2, OUTPUT);
}

// 20 - 200hz Single Pole Bandpass IIR Filter
float bassFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = (sample) / 3.f; // change here to values close to 2, to adapt for stronger or weeker sources of line level audio  
    

    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
    return yv[2];
}

// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(float sample) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample;
    yv[0] = yv[1]; 
    yv[1] = ((xv[0] + xv[1]) / 2.f + (FILTER_ENVELOPE_MULTI * yv[0])) / (1 + FILTER_ENVELOPE_MULTI);
    return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
float beatFilter(float sample) {
    static float xv[3] = {0,0,0};
    static float yv[3] = {0,0,0};
    static double tv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    tv[0] = tv[1]; tv[1] = tv[2]; 
    xv[2] = sample; // / 2.7f;
    tv[2] = millis();
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[0] - xv[1]) / (tv[0] - tv[1]) * 50;
    //yv[2] = (xv[1] - xv[0])
        //+ (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
    return yv[2];
    //return (xv[0] - xv[1]) / (tv[0] - tv[1]) * 50;
}

uint8_t i = 0;

void beat_detector_update(bool beat_state) {
    float value, envelope, beat, thresh;



    // use only bass component
    value = spectrum_analyzer_results.amplitude_left[0];

    // Take signal amplitude and filter
    if(value < 0)value=-value;
    envelope = envelopeFilter(value);

    Serial.print(value);
    Serial.print("\t");
    Serial.print(envelope);
    Serial.print("\t");
    Serial.println(envelopeFilter(beatFilter(envelope)));


    // Every 200 samples (25hz) filter the envelope 
    /*if(i == 200) {
        // Filter out repeating bass sounds 100 - 180bpm
        beat = beatFilter(envelope);
        //Serial.println(beat);
        // Threshold it based on potentiometer on AN1
        thresh = 0.02f * 500;

        // If we are above threshold, light up LED
        if(beat > thresh) {
                    
            beat_state = true;
        } 
        else beat_state = false;

        //Reset sample counter
        i = 0;
    }

        

    // Consume excess clock cycles, to keep at 5000 hz
    for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros());
    */
}