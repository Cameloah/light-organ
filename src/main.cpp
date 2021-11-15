#include <Arduino.h>
#include <FastLED.h>
#include "modules/twinkle.h"
#include "modules/ocean.h"
#include "modules/music_visualisation.h"
#include "led_config.h"

#include "tools/filters.h"
#include "tools/loop_timer.h"

#ifdef ESP32
#include "wifilight.h"  // the wifi light feature requires a ESP32 development board
#endif

// debug and system control options
#define DEBUG_DISPLAY_LOOP_FRQ          // use this to output the loop freq in hz via serial print
#define SYSCTRL_LOOPTIMER               // enable loop frequency control, remember to also set the loop freq in the loop_timer.h
  
// create the FastLED array containing led colors
CRGBArray<LED_NUM_BASS_1> led_arr_base_left;
CRGBArray<LED_NUM_BASS_2> led_arr_base_right;
CRGBArray<LED_NUM_MID> led_arr_mid;
CRGBArray<LED_NUM_TREBLE_1> led_arr_treble_left;
CRGBArray<LED_NUM_TREBLE_2> led_arr_treble_right;

// save led objects in struct for easy parsing to modules
LED_MUSHROOMS_SET_t led_array_set = {
  led_arr_base_left,
  led_arr_base_right,
  led_arr_mid,
  led_arr_treble_left,
  led_arr_treble_right,
};

// restrain the freq. in which the leds are updated to avoid artifacts
#define FREQ_LED_UPDATE_HZ            FREQ_LOOP_CYCLE_HZ

bool state_machine_ambient_flag = false;

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

#ifdef ESP32
    // init wifi module
    wifilight_init(led_arr_mid);
#endif

}


void loop() {

  // save t_0 time stamp in loop_timer
  t_0 = micros();

#ifdef ESP32
    // run wifi update routine
    wifilight_update();
#endif

  EVERY_N_MILLISECONDS(1000 / FREQ_LED_UPDATE_HZ) {

        // state machine
        if(!lights[0].lightState && !lights[1].lightState) {
            // both modes off, clear all leds
            fadeToBlackBy(led_arr_mid, led_arr_mid.size(), 1);
            fadeToBlackBy(led_arr_base_left, led_arr_base_left.size(), 1);
            fadeToBlackBy(led_arr_base_right, led_arr_base_right.size(), 1);
            fadeToBlackBy(led_arr_treble_left, led_arr_treble_left.size(), 1);
            fadeToBlackBy(led_arr_treble_right, led_arr_treble_right.size(), 1);

            state_machine_ambient_flag = false; // mark ambient light mode as default
        }

        else if (lights[0].lightState && !lights[1].lightState) {
            // light organ active
            music_vis_update();
            state_machine_ambient_flag = false; // mark current state
        }

        else if (!lights[0].lightState && lights[1].lightState) {
            // ambient light active
            twinkle_update(led_arr_base_left);
            twinkle_update(led_arr_base_right);
            ocean_update(led_arr_mid);
            twinkle_update(led_arr_treble_right);
            twinkle_update(led_arr_treble_left);
            state_machine_ambient_flag = true; // mark current state
        }

        else {
            // both modes have been activated
            if (state_machine_ambient_flag == false)
                // ambient light has been activated while light organ was still on
                lights[0].lightState = 0;   // turn off light organ

            else
                // light organ has been activated while ambient was still on
                lights[1].lightState = 0;   // turn off ambient
        }
  }

  // execute led colors
  FastLED.show();

  loop_timer++;

#ifdef DEBUG_DISPLAY_LOOP_FRQ
  EVERY_N_SECONDS (5){
    // calculate the loop time and print. serial comm only decreases loop freq when serial.print is called
    Serial.print("loop freq in Hz: ");
    Serial.println(loop_timer_get_loop_freq());
  }
#endif

#ifdef SYSCTRL_LOOPTIMER
  // keep loop at constant cycle frequency
  loop_timer_check_cycle_freq();
#endif

}
    