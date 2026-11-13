#pragma once

#include <FastLED.h>

// pin each LED strip is connected to
#define LED_PIN_BASS_1 23
#define LED_PIN_BASS_2 22
#define LED_PIN_MID 19
#define LED_PIN_TREBLE_1 5
#define LED_PIN_TREBLE_2 4

// number of LEDs on each strip
#define LED_NUM_BASS_1 31
#define LED_NUM_BASS_2 29
#define LED_NUM_MID 20
#define LED_NUM_TREBLE_1 9
#define LED_NUM_TREBLE_2 6

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

// the colors of all LED strips: one array per group of mushrooms
typedef struct {
    CRGBArray<LED_NUM_BASS_1> leds_largeshrooms_left;
    CRGBArray<LED_NUM_BASS_2> leds_largeshrooms_right;
    CRGBArray<LED_NUM_MID> leds_whiteshrooms;
    CRGBArray<LED_NUM_TREBLE_1> leds_redshrooms_left;
    CRGBArray<LED_NUM_TREBLE_2> leds_redshrooms_right;
} LED_MUSHROOMS_SET_t;
