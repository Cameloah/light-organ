#include <Arduino.h>
#include <FastLED.h>
#include "modules/twinkle.h"
#include "modules/ocean.h"
#include "led_config.h"



  
//create the FastLED array containing led colors
CRGBArray<LED_NUM_BASS_1> led_arr_bass_left;
CRGBArray<LED_NUM_BASS_2> led_arr_bass_right;
CRGBArray<LED_NUM_MID> led_arr_mid;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_left;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_right;



void setup() {
  //initialize serial communication
  Serial.begin(9600);

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
  
  EVERY_N_MILLISECONDS(20) {
    twinkle_update(led_arr_bass_left);
    twinkle_update(led_arr_mid);

    twinkle_update(led_arr_bass_right);
    twinkle_update(led_arr_treble_right);
    twinkle_update(led_arr_treble_left);
  }

  //execute led colors
  FastLED.show();
}
    