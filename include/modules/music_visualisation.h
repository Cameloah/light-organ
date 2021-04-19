#include <FastLED.h>
#include "led_config.h"
#include "tools/spectrum_analyzer.h"
#include "tools/filters.h"


#define MUSIC_VIS_SCALE_LIMIT_BASE          400
#define MUSIC_VIS_SCALE_LIMIT_MID           700
/// \brief initializer for music visualisation
///
void music_vis_init();

/// \brief updater for music visualisation
/// \param leds_bass_left reference to leds displaying the left bass channel
/// \param leds_bass_right reference to leds displaying the right bass channel
/// \param leds_mid reference to leds displaying the left bass channel
/// \param leds_treble_right reference to leds displaying the right treble channel
/// \param leds_treble_left reference to leds displaying the left treble channel
///
void music_vis_update(CRGBSet& leds_bass_left,
                      CRGBSet& leds_bass_right,
                      CRGBSet& leds_mid,
                      CRGBSet& leds_treble_left,
                      CRGBSet& leds_treble_right
                      );