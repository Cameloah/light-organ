#include <FastLED.h>
#include "led_config.h"
#include "tools/filters.h"



// base frequency processing
#define MUSIC_VIS_SCALE_LIMIT_BASE          400
#define MUSIC_VIS_BASE_POW_GAIN             181.
#define MUSIC_VIS_BASE_POW_EXPONENT         4

// mid frequency processing
#define MUSIC_VIS_SCALE_LIMIT_MID           700
#define MUSIC_VIS_MID_POW_GAIN              100.
#define MUSIC_VIS_MID_POW_EXPONENT          3

// treble frequency processing
#define MUSIC_VIS_SCALE_LIMIT_TREBLE        300
#define MUSIC_VIS_TREBLE_POW_GAIN           100.
#define MUSIC_VIS_TREBLE_POW_EXPONENT       3

// trigger for led arrays in percent
#define MUSIC_VIS_BASE_TRIGGER              40
#define MUSIC_VIS_TREBLE_TRIGGER            50

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
///
void music_vis_update();

/// \brief handles the individual value processing for the freq bands
///
/// \return the processed value
///
uint16_t music_vis_process_base();
uint16_t music_vis_process_mid();
uint16_t music_vis_process_treble();

