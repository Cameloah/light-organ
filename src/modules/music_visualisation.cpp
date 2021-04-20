#include "modules/music_visualisation.h"
#include "tools/spectrum_analyzer.h"

// buffers to store max values for scaling
float _music_vis_raw_max_input_base           = 0;
float _music_vis_raw_max_input_mid            = 0;
float _music_vis_raw_max_input_treble_l       = 0;
float _music_vis_raw_max_input_treble_r       = 0;

// references for led array set
LED_MUSHROOMS_SET_t* _module_led_set;

void music_vis_init(LED_MUSHROOMS_SET_t* user_buffer) {
    // import user buffer
    // memcpy((uint8_t*) _led_array_set, (uint8_t*) user_buffer, sizeof(LED_MUSHROOMS_SET_t));
    _module_led_set = user_buffer;
    // initialize the spectrumanalyzer arduino shield
    spectrum_analyzer_init();
}

void music_vis_update() {
    // buffer for processed input values
    MUSIC_VIS_VOLUME_t volume_arr;

    // read the amplitudes of the 7-frequency band
    spectrum_read_frequencies();

    // process base, mid and treble
    music_vis_process_base(&volume_arr.volume_base);
    music_vis_process_mid(&volume_arr.volume_mid);
    music_vis_process_treble(&volume_arr.volume_treble_left, &volume_arr.volume_treble_right);

    // animate leds in large shrooms
    music_vis_animation_largeshrooms(&volume_arr.volume_base);

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
}

void music_vis_process_treble(uint16_t* input_left, uint16_t* input_right) {
    // select both stereo channels and presave them in buffers
    *input_left = spectrum_analyzer_results.amplitude_left[6];
    *input_right = spectrum_analyzer_results.amplitude_right[6];
    // scale the measured amplitudes to a max of 1023
    filters_scale(input_left, _music_vis_raw_max_input_treble_l, MUSIC_VIS_SCALE_LIMIT_TREBLE);
    filters_scale(input_right, _music_vis_raw_max_input_treble_r, MUSIC_VIS_SCALE_LIMIT_TREBLE);
    // increase the peak
    *input_left = (uint16_t)pow(*input_left / MUSIC_VIS_TREBLE_POW_DIVIDER, MUSIC_VIS_TREBLE_POW_EXPONENT);
    *input_right = (uint16_t)pow(*input_right / MUSIC_VIS_TREBLE_POW_DIVIDER, MUSIC_VIS_TREBLE_POW_EXPONENT);
}

void music_vis_animation_largeshrooms(uint16_t* input) {
    //display base for RnD purposes
    if(*input > (uint16_t) MUSIC_VIS_BASE_TRIGGER * 10.24) {
        _module_led_set->leds_largeshrooms_left = CRGB::Red;
        _module_led_set->leds_largeshrooms_right = CRGB::Red;
    }

    else {
        fade_raw(_module_led_set->leds_largeshrooms_left, _module_led_set->leds_largeshrooms_left.size(), MUSIC_VIS_BASE_FADE);
        fade_raw(_module_led_set->leds_largeshrooms_right, _module_led_set->leds_largeshrooms_right.size(), MUSIC_VIS_BASE_FADE);
    }
}