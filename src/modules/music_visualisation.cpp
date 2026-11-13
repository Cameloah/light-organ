#include "modules/music_visualisation.h"

#include "modules/led_effects.h"
#include "tools/filters.h"
#include "tools/spectrum_analyzer.h"

// debug and system control options
// #define DEBUG_DISPLAY_BASS_VALUES
// #define DEBUG_DISPLAY_MID_VALUES

// bass frequency processing
#define MUSIC_VIS_SCALE_LIMIT_BASS          400
#define MUSIC_VIS_BASS_POW_DIVIDER          181.
#define MUSIC_VIS_BASS_POW_EXPONENT         4

// mid frequency processing
#define MUSIC_VIS_SCALE_LIMIT_MID           700
#define MUSIC_VIS_MID_POW_DIVIDER           100.
#define MUSIC_VIS_MID_POW_EXPONENT          3

// treble frequency processing
#define MUSIC_VIS_SCALE_LIMIT_TREBLE        300
#define MUSIC_VIS_TREBLE_POW_DIVIDER        100.
#define MUSIC_VIS_TREBLE_POW_EXPONENT       3

// trigger for led arrays in percent
#define MUSIC_VIS_BASS_TRIGGER              40
#define MUSIC_VIS_TREBLE_TRIGGER            50

// fade integer by which the colors are faded back to black
#define MUSIC_VIS_BASS_FADE                 5
#define MUSIC_VIS_BASS_DELAYTOSPARK_MS      1000
#define MUSIC_VIS_BASS_DELAYINTERSPARK_MS   20
#define MUSIC_VIS_BASS_TIMESPARKLES_MS      1500
#define MUSIC_VIS_BASS_SPARKLE_FADE         30

#define MUSIC_VIS_TREBLE_FADE               2

static_assert(LED_NUM_BASS_1 <= LED_EFFECTS_SPARKLE_STATE_ARR_SIZE &&
                  LED_NUM_BASS_2 <= LED_EFFECTS_SPARKLE_STATE_ARR_SIZE,
              "the sparkle effect keeps one state per LED");

typedef struct
{
    uint16_t volume_bass;
    uint16_t volume_mid;
    uint16_t volume_treble_left;
    uint16_t volume_treble_right;
} MUSIC_VIS_VOLUME_t;

static void music_vis_process_bass(uint16_t* input);
static void music_vis_process_mid(uint16_t* input);
static void music_vis_process_treble(uint16_t* input_left, uint16_t* input_right);
static void music_vis_animation_largeshrooms(uint16_t* input);
static void music_vis_animation_whiteshrooms(uint16_t* input);
static void music_vis_animation_redshrooms(uint16_t* input_left, uint16_t* input_right);

// buffers to store max values for scaling
static float _music_vis_raw_max_input_bass           = 0;
static float _music_vis_raw_max_input_mid            = 0;
static float _music_vis_raw_max_input_treble_l       = 0;
static float _music_vis_raw_max_input_treble_r       = 0;

// references for led array set
static LED_MUSHROOMS_SET_t* _module_led_set;

static LED_EFFECTS_SPARKLE_t _music_vis_sparkle_bass_left = {
    NULL,
    MUSIC_VIS_BASS_DELAYTOSPARK_MS,
    MUSIC_VIS_BASS_DELAYINTERSPARK_MS,
    0,
    MUSIC_VIS_BASS_TIMESPARKLES_MS,
    MUSIC_VIS_BASS_SPARKLE_FADE,
};

static LED_EFFECTS_SPARKLE_t _music_vis_sparkle_bass_right;

void music_vis_init() {
    // fill second sparkle buffer with data
    memcpy(&_music_vis_sparkle_bass_right, &_music_vis_sparkle_bass_left, sizeof(LED_EFFECTS_SPARKLE_t));
    // initialize the spectrumanalyzer arduino shield
    spectrum_analyzer_init();
}

void music_vis_update(LED_MUSHROOMS_SET_t* user_buffer) {
    // import user buffer
    _module_led_set = user_buffer;

    // assign corresponding led array to sparkle stuct buffer
    _music_vis_sparkle_bass_left.led_array = &_module_led_set->leds_largeshrooms_left;
    _music_vis_sparkle_bass_right.led_array = &_module_led_set->leds_largeshrooms_right;

    // buffer for processed input values
    MUSIC_VIS_VOLUME_t volume_arr;

    // read the amplitudes of the 7-frequency band
    spectrum_read_frequencies();

    // process volume values for bass, mid and treble
    music_vis_process_bass(&volume_arr.volume_bass);
    music_vis_process_mid(&volume_arr.volume_mid);
    music_vis_process_treble(&volume_arr.volume_treble_left, &volume_arr.volume_treble_right);

    // animate leds in shrooms
    music_vis_animation_largeshrooms(&volume_arr.volume_bass);
    music_vis_animation_whiteshrooms(&volume_arr.volume_mid);
    music_vis_animation_redshrooms(&volume_arr.volume_treble_left, &volume_arr.volume_treble_right);

}

static void music_vis_process_bass(uint16_t* input) {
    // select the smaller value from stereo and presave it to buffer
    *input = (spectrum_analyzer_results.amplitude_left[1] > spectrum_analyzer_results.amplitude_right[1])
            ? spectrum_analyzer_results.amplitude_right[1] : spectrum_analyzer_results.amplitude_left[1];
    
    #ifdef DEBUG_DISPLAY_BASS_VALUES
    Serial.print(*input);
    Serial.print("\t");
    #endif

    // scale the measured amplitudes to a max of 1023
    filters_scale(input, _music_vis_raw_max_input_bass, MUSIC_VIS_SCALE_LIMIT_BASS);
    // increase the peak
    *input = (uint16_t) pow((float) *input / MUSIC_VIS_BASS_POW_DIVIDER, MUSIC_VIS_BASS_POW_EXPONENT);

    #ifdef DEBUG_DISPLAY_BASS_VALUES
    Serial.print(*input);
    Serial.print("\t");
    Serial.print(_music_vis_raw_max_input_bass);
    Serial.print("\t");
    Serial.println((uint16_t) MUSIC_VIS_BASS_TRIGGER * 10.24);
    #endif
}

static void music_vis_process_mid(uint16_t* input) {
    // use the loudest band for white mushrooms and presave it to buffer
    *input = spectrum_loudest_band();
    // scale the measured amplitudes to a max of 1023
    filters_scale(input, _music_vis_raw_max_input_mid, MUSIC_VIS_SCALE_LIMIT_MID);
    *input = pow(*input / MUSIC_VIS_MID_POW_DIVIDER, MUSIC_VIS_MID_POW_EXPONENT);
    
    #ifdef DEBUG_DISPLAY_MID_VALUES
    Serial.print(*input);
    Serial.print("\t");
    Serial.println(_music_vis_raw_max_input_mid);
    #endif
}

static void music_vis_process_treble(uint16_t* input_left, uint16_t* input_right) {
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

static void music_vis_animation_largeshrooms(uint16_t* input) {
    // fade all leds before new instructions follow
    fade_raw(_module_led_set->leds_largeshrooms_left, _module_led_set->leds_largeshrooms_left.size(), MUSIC_VIS_BASS_FADE);
    fade_raw(_module_led_set->leds_largeshrooms_right, _module_led_set->leds_largeshrooms_right.size(), MUSIC_VIS_BASS_FADE);

    bool trigger_flag = false;

    // check for trigger
    if(*input > (uint16_t) MUSIC_VIS_BASS_TRIGGER * 10.24) {
        // apply bass color
        _module_led_set->leds_largeshrooms_left = CRGB::Red;
        _module_led_set->leds_largeshrooms_right = CRGB::Red;
        // raise trigger flag
        trigger_flag = true;
    }

    // release some sparkles after a predefined delay
    led_effects_run_sparkle(&_music_vis_sparkle_bass_right, &trigger_flag);
    led_effects_run_sparkle(&_music_vis_sparkle_bass_left, &trigger_flag);
}

static void music_vis_animation_whiteshrooms(uint16_t* input) {
    // map total volume to number of white shrooms
    uint8_t num_leds_triggered = map(*input, 0, 1024, 0, _module_led_set->leds_whiteshrooms.size());
    // use special fade fct
    led_effects_glow_fade(_module_led_set->leds_whiteshrooms, 2, 10);
    // apply some color
    led_effect_fill_gradient_interval(_module_led_set->leds_whiteshrooms, 0, num_leds_triggered, CRGB::Blue, CRGB::Yellow);
}

static void music_vis_animation_redshrooms(uint16_t* input_left, uint16_t* input_right) {
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