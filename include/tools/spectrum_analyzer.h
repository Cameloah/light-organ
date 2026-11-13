#pragma once

#include <Arduino.h>

// number of frequency bands of the analyzer chip (MSGEQ7)
#define SPECTRUM_CH_NUM 7

typedef struct {
    uint16_t amplitude_left[SPECTRUM_CH_NUM];
    uint16_t amplitude_right[SPECTRUM_CH_NUM];
} SPECTRUM_ANALYZER_RAW_BUFFER_t;

extern SPECTRUM_ANALYZER_RAW_BUFFER_t spectrum_analyzer_results;

/// \brief initializes pin configurations and spectrum shield communication
///
void spectrum_analyzer_init();

/// \brief communicates with spectrum shield to pull apmplitudes of 7 band FFT
///
void spectrum_read_frequencies();

/// \brief finds the loudest of all bands on both channels
///
/// \return its amplitude, 0 - 1023
///
uint16_t spectrum_loudest_band();
