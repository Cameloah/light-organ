#include "tools/spectrum_analyzer.h"

// #define DEBUG_DISPLAY_SPECTRUM_AMPLITUDES

// pins of the spectrum shield
static const uint8_t PIN_STROBE = 32;
static const uint8_t PIN_RESET = 33;
static const uint8_t PIN_DC_LEFT = 34;
static const uint8_t PIN_DC_RIGHT = 35;

SPECTRUM_ANALYZER_RAW_BUFFER_t spectrum_analyzer_results;

void spectrum_analyzer_init() {
    // Set spectrum Shield pin configurations
    pinMode(PIN_STROBE, OUTPUT);
    pinMode(PIN_RESET, OUTPUT);
    pinMode(PIN_DC_LEFT, INPUT);
    pinMode(PIN_DC_RIGHT, INPUT);
    digitalWrite(PIN_STROBE, HIGH);
    digitalWrite(PIN_RESET, HIGH);

    // Initialize Spectrum Analyzers
    digitalWrite(PIN_STROBE, LOW);
    delay(1);
    digitalWrite(PIN_RESET, HIGH);
    delay(1);
    digitalWrite(PIN_STROBE, HIGH);
    delay(1);
    digitalWrite(PIN_STROBE, LOW);
    delay(1);
    digitalWrite(PIN_RESET, LOW);
}

void spectrum_read_frequencies() {
    for (int freq_amp = 0; freq_amp < SPECTRUM_CH_NUM; freq_amp++) {
        spectrum_analyzer_results.amplitude_left[freq_amp] =
            map(analogRead(PIN_DC_LEFT), 0, 4096, 0, 1024);
        spectrum_analyzer_results.amplitude_right[freq_amp] =
            map(analogRead(PIN_DC_RIGHT), 0, 4096, 0, 1024);
        digitalWrite(PIN_STROBE, HIGH);
        digitalWrite(PIN_STROBE, LOW);
    }

#ifdef DEBUG_DISPLAY_SPECTRUM_AMPLITUDES
    Serial.print(spectrum_analyzer_results.amplitude_left[5]);
    Serial.print("\t");
    Serial.print(spectrum_analyzer_results.amplitude_right[5]);
    Serial.print("\t");

    Serial.print(spectrum_analyzer_results.amplitude_left[6]);
    Serial.print("\t");
    Serial.println(spectrum_analyzer_results.amplitude_right[6]);
    Serial.print("\t");
#endif
}

uint16_t spectrum_loudest_band() {
    uint16_t greatestVol = 0;
    for (int i = 0; i < SPECTRUM_CH_NUM; i++) {
        if (spectrum_analyzer_results.amplitude_left[i] > greatestVol) {
            greatestVol = spectrum_analyzer_results.amplitude_left[i];
        }
        if (spectrum_analyzer_results.amplitude_right[i] > greatestVol) {
            greatestVol = spectrum_analyzer_results.amplitude_right[i];
        }
    }
    return greatestVol;
}
