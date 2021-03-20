#include <Arduino.h>
#include <FastLED.h>
#include "modules/twinkle.h"
#include "modules/ocean.h"
#include "led_config.h"

#include "tools/spectrum_analyzer.h"
#include "tools/beat_detector.h"
#include "tools/filters.h"
#include "tools/loop_timer.h"



  
//create the FastLED array containing led colors
CRGBArray<LED_NUM_BASS_1> led_arr_bass_left;
CRGBArray<LED_NUM_BASS_2> led_arr_bass_right;
CRGBArray<LED_NUM_MID> led_arr_mid;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_left;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_right;

#define FREQ_LED_UPDATE_HZ            50

//time buffers
#define FREQ_LOOP_CYCLE_HZ            100
uint64_t t_0 = 0;
uint64_t t_end = 0;

void setup() {
  //initialize serial communication
  Serial.begin(115200);

  spectrum_analyzer_init();

  //initialize effect modules
  twinkle_init();


  //start up FastLED object
  FastLED.addLeds<LED_TYPE, LED_PIN_BASS_1, COLOR_ORDER>(led_arr_bass_left, LED_NUM_BASS_1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_BASS_2, COLOR_ORDER>(led_arr_bass_right, LED_NUM_BASS_2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_MID, COLOR_ORDER>(led_arr_mid, LED_NUM_MID).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_TREBLE_1, COLOR_ORDER>(led_arr_treble_left, LED_NUM_TREBLE_1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_TREBLE_2, COLOR_ORDER>(led_arr_treble_right, LED_NUM_TREBLE_2).setCorrection(TypicalLEDStrip);

}


void loop() {

  //save t_0 time stamp
  t_0 = micros();

  spectrum_read_frequencies();

  EVERY_N_MILLISECONDS(1000 / FREQ_LED_UPDATE_HZ) {
    twinkle_update(led_arr_bass_left);
    twinkle_update(led_arr_bass_right);

    ocean_update(led_arr_mid);

    twinkle_update(led_arr_treble_right);
    twinkle_update(led_arr_treble_left);
  }

  beat_detector_update(1);

  //execute led colors
  FastLED.show();

  loop_timer++;
  EVERY_N_SECONDS (5){
    Serial.print("loop freq in Hz: ");
    Serial.println(get_loop_freq());

  }

  //keep loop at constant cycle frequency
  t_end = micros();
  uint64_t t_delta = t_end - t_0;
  if(t_delta < (1000000 / FREQ_LOOP_CYCLE_HZ)) {
    delayMicroseconds((1000000 / FREQ_LOOP_CYCLE_HZ) - t_delta);
  }
}
    