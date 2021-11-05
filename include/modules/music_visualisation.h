#include <FastLED.h>
#include "led_config.h"
#include "modules/led_effects.h"
#include "tools/filters.h"

// debug and system control options
// #define DEBUG_DISPLAY_BASE_VALUES
// #define DEBUG_DISPLAY_MID_VALUES
// base frequency processing
#define MUSIC_VIS_SCALE_LIMIT_BASE          400
#define MUSIC_VIS_BASE_POW_DIVIDER          181.
#define MUSIC_VIS_BASE_POW_EXPONENT         4

// mid frequency processing
#define MUSIC_VIS_SCALE_LIMIT_MID           700
#define MUSIC_VIS_MID_POW_DIVIDER           100.
#define MUSIC_VIS_MID_POW_EXPONENT          3

// treble frequency processing
#define MUSIC_VIS_SCALE_LIMIT_TREBLE        300
#define MUSIC_VIS_TREBLE_POW_DIVIDER        100.
#define MUSIC_VIS_TREBLE_POW_EXPONENT       3

// trigger for led arrays in percent
#define MUSIC_VIS_BASE_TRIGGER              40
#define MUSIC_VIS_TREBLE_TRIGGER            50

// fade integer by which the colors are faded back to black
#define MUSIC_VIS_BASE_FADE                 5
#define MUSIC_VIS_BASE_DELAYTOSPARK_MS      1000
#define MUSIC_VIS_BASE_DELAYINTERSPARK_MS   20
#define MUSIC_VIS_BASE_TIMESPARKLES_MS      1500
#define MUSIC_VIS_BASE_SPARKLE_FADE         30

#define MUSIC_VIS_TREBLE_FADE               2


typedef struct
{
    CRGBSet& leds_largeshrooms_left;
    CRGBSet& leds_largeshrooms_right;
    CRGBSet& leds_whiteshrooms;
    CRGBSet& leds_redshrooms_left;
    CRGBSet& leds_redshrooms_right;
} LED_MUSHROOMS_SET_t;

typedef struct
{
    uint16_t volume_base;
    uint16_t volume_mid;
    uint16_t volume_treble_left;
    uint16_t volume_treble_right;
} MUSIC_VIS_VOLUME_t;


/// \brief initializer for music visualisation
///
/// \param user_buffer structs of references of the 5 different led arrays
///
void music_vis_init(LED_MUSHROOMS_SET_t* user_buffer);

/// \brief updater for music visualisation
/// calls all necessary functions to create the light effects using the amplitudes
/// from the spectrum analyzer
///
void music_vis_update();

/// \brief handles the individual value processing for the freq bands
///
/// \param input pointer to respective buffer for processed data
///
void music_vis_process_base(uint16_t* input);
void music_vis_process_mid(uint16_t* input);
void music_vis_process_treble(uint16_t* input_left, uint16_t* input_right);

/// \brief handles the animation of the respective led arrays by computing the colors for the leds
///
void music_vis_animation_largeshrooms(uint16_t* input);
void music_vis_animation_whiteshrooms(uint16_t* input);
void music_vis_animation_redshrooms(uint16_t* input_left, uint16_t* input_right);