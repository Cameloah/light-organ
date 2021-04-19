#include <Arduino.h>
#include <FastLED.h>
#include "modules/twinkle.h"
#include "modules/ocean.h"
#include "modules/music_visualisation.h"
#include "led_config.h"

#include "tools/filters.h"
#include "tools/loop_timer.h"

// debug and system control options
#define DEBUG_DISPLAY_LOOP_FRQ          // use this to output the loop freq in hz via serial print
#define SYSCTRL_LOOPTIMER               // enable loop frequency control, remember to also set the loop freq in the loop_timer.h
  
// create the FastLED array containing led colors
CRGBArray<LED_NUM_BASS_1> led_arr_base_left;
CRGBArray<LED_NUM_BASS_2> led_arr_base_right;
CRGBArray<LED_NUM_MID> led_arr_mid;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_left;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_right;

// save led objects in struct for easy parsing to modules
LED_MUSHROOMS_SET_t led_array_set = {
  led_arr_base_left,
  led_arr_base_right,
  led_arr_mid,
  led_arr_treble_left,
  led_arr_treble_right,
};

// restrain the freq. in which the leds are updated to avoid artifacts
#define FREQ_LED_UPDATE_HZ            50

void setup() {
  // initialize serial communication
  Serial.begin(115200);

  // initialize effect modules
  twinkle_init();
  music_vis_init(&led_array_set);

  //start up FastLED object
  FastLED.addLeds<LED_TYPE, LED_PIN_BASS_1, COLOR_ORDER>(led_arr_base_left, LED_NUM_BASS_1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_BASS_2, COLOR_ORDER>(led_arr_base_right, LED_NUM_BASS_2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_MID, COLOR_ORDER>(led_arr_mid, LED_NUM_MID).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_TREBLE_1, COLOR_ORDER>(led_arr_treble_left, LED_NUM_TREBLE_1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN_TREBLE_2, COLOR_ORDER>(led_arr_treble_right, LED_NUM_TREBLE_2).setCorrection(TypicalLEDStrip);

}


void loop() {

  // save t_0 time stamp in loop_timer
  t_0 = micros();

  EVERY_N_MILLISECONDS(1000 / FREQ_LED_UPDATE_HZ) {
    /*
    twinkle_update(led_arr_base_left);
    twinkle_update(led_arr_base_right);

    ocean_update(led_arr_mid);

    twinkle_update(led_arr_treble_right);
    twinkle_update(led_arr_treble_left);
    */

    music_vis_update();
  }

  // execute led colors
  FastLED.show();

  loop_timer++;

  #ifdef DEBUG_DISPLAY_LOOP_FRQ
  EVERY_N_SECONDS (5){
    Serial.print("loop freq in Hz: ");
    Serial.println(loop_timer_get_loop_freq());
  }
  #endif

  // keep loop at constant cycle frequency
  loop_timer_check_cycle_freq();
}
    