#include <FastLED.h>
#include "led_config.h"

//pin microphone input is connected to
#define ANALOG_READ                                     1

//set color increments
#define PEAK_INC_R                                      -1
#define PEAK_INC_G                                      4
#define PEAK_INC_B                                      20

#define BASE_INC_R                                      10
#define BASE_INC_G                                      4
#define BASE_INC_B                                      -4

//set color maxima
#define PEAK_MAX_R                                      255
#define PEAK_MAX_G                                      255
#define PEAK_MAX_B                                      255

#define BASE_MAX_R                                      255
#define BASE_MAX_G                                      180
#define BASE_MAX_B                                      100


typedef struct
{
    int r;
    int g;
    int b;
} color_increments_t;




/// /brief initializer for music visualisation
///
void music_vis_init();

/// /brief updater for music visualisation
///
void music_vis_update(CRGBSet& leds);