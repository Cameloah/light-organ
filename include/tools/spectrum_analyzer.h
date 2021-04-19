#include <Arduino.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define STROBE                      4
#define RESET                       5
#define DC_left                     A0
#define DC_right                    A1

#define SPECTRUM_CH_NUM             7

typedef struct {
    uint16_t amplitude_left[SPECTRUM_CH_NUM];
    uint16_t amplitude_right[SPECTRUM_CH_NUM];
} spectrum_analyzer_raw_buffer;

extern spectrum_analyzer_raw_buffer spectrum_analyzer_results;



/// \brief initializes pin configurations and spectrum shield communication
///
void spectrum_analyzer_init();

/// \brief communicates with spectrum shield to pull apmplitudes of 7 band FFT
///
void spectrum_read_frequencies();

/// \brief computes the total volume from all 7 freq bands
/// \return total volume 0 - 1023
uint16_t spectrum_total_volume();