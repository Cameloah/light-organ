#include "modules/music_visualisation.h"
#include "tools/spectrum_analyzer.h"

// buffers to store max values for scaling
float _music_vis_raw_max_input_base           = 0;
float _music_vis_raw_max_input_mid            = 0;
float _music_vis_raw_max_input_treble_l       = 0;
float _music_vis_raw_max_input_treble_r       = 0;

// references for led array set
LED_MUSHROOMS_SET_t* _module_led_set;

LED_EFFECTS_SPARKLE_t _music_vis_sparkle_base_left = {
    NULL,
    MUSIC_VIS_BASE_DELAYTOSPARK_MS,
    MUSIC_VIS_BASE_DELAYINTERSPARK_MS,
    0,
    MUSIC_VIS_BASE_TIMESPARKLES_MS,
    MUSIC_VIS_BASE_SPARKLE_FADE,
};

LED_EFFECTS_SPARKLE_t _music_vis_sparkle_base_right;

void music_vis_init() {
    // fill second sparkle buffer with data
    memcpy(&_music_vis_sparkle_base_right, &_music_vis_sparkle_base_left, sizeof(LED_EFFECTS_SPARKLE_t));
    // initialize the spectrumanalyzer arduino shield
    spectrum_analyzer_init();
}

void music_vis_update(LED_MUSHROOMS_SET_t* user_buffer) {
    // import user buffer
    _module_led_set = user_buffer;

    // assign corresponding led array to buffer
    _music_vis_sparkle_base_left.led_array = &_module_led_set->leds_largeshrooms_left;
    _music_vis_sparkle_base_right.led_array = &_module_led_set->leds_largeshrooms_right;

    // buffer for processed input values
    MUSIC_VIS_VOLUME_t volume_arr;

    // read the amplitudes of the 7-frequency band
    spectrum_read_frequencies();

    // process base, mid and treble
    music_vis_process_base(&volume_arr.volume_base);
    music_vis_process_mid(&volume_arr.volume_mid);
    music_vis_process_treble(&volume_arr.volume_treble_left, &volume_arr.volume_treble_right);

    // animate leds in shrooms
    music_vis_animation_largeshrooms(&volume_arr.volume_base);
    music_vis_animation_whiteshrooms(&volume_arr.volume_mid);
    music_vis_animation_redshrooms(&volume_arr.volume_treble_left, &volume_arr.volume_treble_right);

}

void music_vis_process_base(uint16_t* input) {
    // select the smaller value from stereo and presave it to buffer
    *input = (spectrum_analyzer_results.amplitude_left[1] > spectrum_analyzer_results.amplitude_right[1])
            ? spectrum_analyzer_results.amplitude_right[1] : spectrum_analyzer_results.amplitude_left[1];
    
    #ifdef DEBUG_DISPLAY_BASE_VALUES
    Serial.print(*input);
    Serial.print("\t");
    #endif

    // scale the measured amplitudes to a max of 1023
    filters_scale(input, _music_vis_raw_max_input_base, MUSIC_VIS_SCALE_LIMIT_BASE);
    // increase the peak
    *input = (uint16_t) pow((float) *input / 181., MUSIC_VIS_BASE_POW_EXPONENT);

    #ifdef DEBUG_DISPLAY_BASE_VALUES
    Serial.print(*input);
    Serial.print("\t");
    Serial.print(_music_vis_raw_max_input_base);
    Serial.print("\t");
    Serial.println((uint16_t) MUSIC_VIS_BASE_TRIGGER * 10.24);
    #endif
}

void music_vis_process_mid(uint16_t* input) {
    // use total volume for white mushrooms and presave it to buffer
    *input = spectrum_total_volume();
    // scale the measured amplitudes to a max of 1023
    filters_scale(input, _music_vis_raw_max_input_mid, MUSIC_VIS_SCALE_LIMIT_MID);
    *input = pow(*input / MUSIC_VIS_MID_POW_DIVIDER, MUSIC_VIS_MID_POW_EXPONENT);
    
    #ifdef DEBUG_DISPLAY_MID_VALUES
    Serial.print(*input);
    Serial.print("\t");
    Serial.println(_music_vis_raw_max_input_mid);
    #endif
}

void music_vis_process_treble(uint16_t* input_left, uint16_t* input_right) {
    // select both stereo channels and presave them in buffers
    *input_left = spectrum_analyzer_results.amplitude_left[5];
    *input_right = spectrum_analyzer_results.amplitude_right[5];
    // scale the measured amplitudes to a max of 1023
    filters_scale(input_left, _music_vis_raw_max_input_treble_l, MUSIC_VIS_SCALE_LIMIT_TREBLE);
    filters_scale(input_right, _music_vis_raw_max_input_treble_r, MUSIC_VIS_SCALE_LIMIT_TREBLE);
    // increase the peak
    *input_left = (uint16_t)pow(*input_left / MUSIC_VIS_TREBLE_POW_DIVIDER, MUSIC_VIS_TREBLE_POW_EXPONENT);
    *input_right = (uint16_t)pow(*input_right / MUSIC_VIS_TREBLE_POW_DIVIDER, MUSIC_VIS_TREBLE_POW_EXPONENT);
}

void music_vis_animation_largeshrooms(uint16_t* input) {
    // fade all leds before new instructions follow
    fade_raw(_module_led_set->leds_largeshrooms_left, _module_led_set->leds_largeshrooms_left.size(), MUSIC_VIS_BASE_FADE);
    fade_raw(_module_led_set->leds_largeshrooms_right, _module_led_set->leds_largeshrooms_right.size(), MUSIC_VIS_BASE_FADE);

    bool trigger_flag = false;

    // check for trigger
    if(*input > (uint16_t) MUSIC_VIS_BASE_TRIGGER * 10.24) {
        // apply base color
        _module_led_set->leds_largeshrooms_left = CRGB::Red;
        _module_led_set->leds_largeshrooms_right = CRGB::Red;
        // raise trigger flag
        trigger_flag = true;
    }

    // release some sparkles after a predefined delay
    led_effects_run_sparkle(&_music_vis_sparkle_base_right, &trigger_flag);
    led_effects_run_sparkle(&_music_vis_sparkle_base_left, &trigger_flag);
}

void music_vis_animation_whiteshrooms(uint16_t* input) {
    // map total volume to number of white shrooms
    uint8_t num_leds_triggered = map(*input, 0, 1024, 0, _module_led_set->leds_whiteshrooms.size());
    // use special fade fct
    led_effects_glow_fade(_module_led_set->leds_whiteshrooms, 2, 10);
    // apply some color
    led_effect_fill_gradient_interval(_module_led_set->leds_whiteshrooms, 0, num_leds_triggered, CRGB::Blue, CRGB::Yellow);
}

void music_vis_animation_redshrooms(uint16_t* input_left, uint16_t* input_right) {
    // fade all leds before new instructions follow
    fade_raw(_module_led_set->leds_redshrooms_left, _module_led_set->leds_redshrooms_left.size(), MUSIC_VIS_TREBLE_FADE);
    fade_raw(_module_led_set->leds_redshrooms_right, _module_led_set->leds_redshrooms_right.size(), MUSIC_VIS_TREBLE_FADE);

    // check for trigger
    if(*input_left > (uint16_t) MUSIC_VIS_TREBLE_TRIGGER * 10.24) {
        // apply some color
        _module_led_set->leds_redshrooms_left = CRGB::Red;
    }

    if(*input_right > (uint16_t) MUSIC_VIS_TREBLE_TRIGGER * 10.24) {
        // apply some color
        _module_led_set->leds_redshrooms_right = CRGB::Red;
    }
}