#define FASTLED_ESP32_I2S true // keeps esp from crashing when using FastLED and wifi at the same time

#include "main.h"

#include <FastLED.h>

#include "modules/twinkle.h"
#include "modules/ocean.h"
#include "modules/user_interface.h"
#include "modules/dmx_manager.h"
#include "modules/control.h"

#include "led_config.h"
#include "tools/filters.h"
#include "tools/loop_timer.h"

#include "github_update.h"
#include "memory_module.h"
#include "ram_log.h"

// ------------------ SYSTEM -------------------- //

TaskHandle_t Task_network;

void loop_core_0(void* parameter) {
    for(;;) {
        // run wifi update routine
        project_utils_update();
        ui_serial_comm_handler();
        //dmx_listen();
        control_update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


// --------------- LED VARIABLES ---------------- //

// declare memory for the led colors
LED_MUSHROOMS_SET_t led_array_set_real;         // contains the values that get send to the leds
LED_MUSHROOMS_SET_t led_array_set_current;      // contains the values generated by the current module
LED_MUSHROOMS_SET_t led_array_set_previous;     // contains the values of the next module in line

uint8_t blend_opacity = 0;
uint8_t module_index_current = 0;
uint8_t module_index_previous = 0;

#define EFFECT_MODULE_NUM                     3

void animations_update(LED_MUSHROOMS_SET_t *set) {
    twinkle_update(reinterpret_cast<CRGBSet &>(*reinterpret_cast<CRGB *>(&set->leds_largeshrooms_left)));
    twinkle_update(reinterpret_cast<CRGBSet &>(*reinterpret_cast<CRGB *>(&set->leds_largeshrooms_right)));
    ocean_update(reinterpret_cast<CRGBSet &>(*reinterpret_cast<CRGB *>(&set->leds_whiteshrooms)));
    twinkle_update(reinterpret_cast<CRGBSet &>(*reinterpret_cast<CRGB *>(&set->leds_redshrooms_left)));
    twinkle_update(reinterpret_cast<CRGBSet &>(*reinterpret_cast<CRGB *>(&set->leds_redshrooms_right)));
}

void blackout_update(LED_MUSHROOMS_SET_t *set) {
    set->leds_largeshrooms_left = CRGB::Black;
    set->leds_largeshrooms_right = CRGB::Black;
    set->leds_whiteshrooms = CRGB::Black;
    set->leds_redshrooms_left = CRGB::Black;
    set->leds_redshrooms_right = CRGB::Black;
}

// holds effects that program will cycle through
void (*module_update[EFFECT_MODULE_NUM])(LED_MUSHROOMS_SET_t *set) = {
        blackout_update,
        music_vis_update,
        animations_update,
};

void led_mode_switch(uint8_t module_index) {
    led_array_set_previous = led_array_set_real;
    blackout_update(&led_array_set_current);
    module_index_previous = module_index_current;
    module_index_current = module_index;
    blend_opacity = 0;
}



// ---------------- MAIN SETUP ------------------- //

void setup() {

    // ---------- system ----------- //
    DualSerial.begin(115200);

    // --------  led setup --------- //

    // initialize effect modules
    twinkle_init();
    music_vis_init();
    //dmx_init();

    //start up FastLED object
    FastLED.addLeds<LED_TYPE, LED_PIN_BASS_1, COLOR_ORDER>(led_array_set_real.leds_largeshrooms_left,
                                                           LED_NUM_BASS_1).setCorrection(
                                                            TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN_BASS_2, COLOR_ORDER>(led_array_set_real.leds_largeshrooms_right,
                                                           LED_NUM_BASS_2).setCorrection(
                                                            TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN_MID, COLOR_ORDER>(led_array_set_real.leds_whiteshrooms,
                                                        LED_NUM_MID).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN_TREBLE_1, COLOR_ORDER>(led_array_set_real.leds_redshrooms_left,
                                                             LED_NUM_TREBLE_1).setCorrection(
                                                                     TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_PIN_TREBLE_2, COLOR_ORDER>(led_array_set_real.leds_redshrooms_right,
                                                             LED_NUM_TREBLE_2).setCorrection(
                                                                     TypicalLEDStrip);

    // set all leds to black
    blackout_update(&led_array_set_real);
    FastLED.show();


    // --------- network ---------- //

    DualSerial.println("Starting Wifi...");
    project_utils_init("Magic Light Organ");
    control_init();
    

    // ------- task setup --------- // 
    
    //everything network related will run on core 0, this will start the task
    xTaskCreatePinnedToCore(
        loop_core_0,
        "Task_network",
        8192,
        NULL,
        1,
        &Task_network,
        0);

    delay(2000);
}


void loop() {
    // ---------- system ----------- //

    // save t_0 time stamp in loop_timer
    t_0 = micros();


    // ----------- leds ------------ //

    if (blend_opacity < 255) {
        module_update[module_index_previous](&led_array_set_previous);
        module_update[module_index_current](&led_array_set_current);

        blend(led_array_set_previous.leds_largeshrooms_left, led_array_set_current.leds_largeshrooms_left,
            led_array_set_real.leds_largeshrooms_left, LED_NUM_BASS_1, blend_opacity);
        blend(led_array_set_previous.leds_largeshrooms_right, led_array_set_current.leds_largeshrooms_right,
            led_array_set_real.leds_largeshrooms_right, LED_NUM_BASS_2, blend_opacity);
        blend(led_array_set_previous.leds_whiteshrooms, led_array_set_current.leds_whiteshrooms,
            led_array_set_real.leds_whiteshrooms, LED_NUM_MID, blend_opacity);
        blend(led_array_set_previous.leds_redshrooms_left, led_array_set_current.leds_redshrooms_left,
            led_array_set_real.leds_redshrooms_left, LED_NUM_TREBLE_1, blend_opacity);
        blend(led_array_set_previous.leds_redshrooms_right, led_array_set_current.leds_redshrooms_right,
            led_array_set_real.leds_redshrooms_right, LED_NUM_TREBLE_2, blend_opacity);

        blend_opacity = (blend_opacity + 3) > 255 ? 255 : (blend_opacity + 3);
    }

    else module_update[module_index_current](&led_array_set_real);

    // execute led colors
    FastLED.show();


/*
    EVERY_N_SECONDS(5) {
        static bool ambient_mode = true;
        if (ambient_mode) {
            DualSerial.println("switching to ambient mode");
            led_mode_switch(MODE_AMBIENT);
        } else {
            DualSerial.println("switching to music mode");
            led_mode_switch(MODE_MUSIC);
        }
        ambient_mode = !ambient_mode;
    }*/

    // ---------- system ----------- //

    loop_timer++;

#ifdef DEBUG_DISPLAY_LOOP_FRQ
    EVERY_N_SECONDS (5) {
        // calculate the loop time and print. serial comm only decreases loop freq when serial.print is called
        DualSerial.print("loop freq in Hz: ");
        DualSerial.println(loop_timer_get_loop_freq());
    }
#endif

#ifdef SYSCTRL_LOOPTIMER
    // keep loop at constant cycle frequency
    loop_timer_check_cycle_freq();
#endif
}
    