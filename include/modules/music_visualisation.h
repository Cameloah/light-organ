#pragma once

#include "led_config.h"

/// \brief initializer for music visualisation, sets up the spectrum analyzer
///
void music_vis_init();

/// \brief updater for music visualisation
/// reads the amplitudes from the spectrum analyzer and computes the colors of all LEDs
///
/// \param user_buffer the LED arrays to draw into
///
void music_vis_update(LED_MUSHROOMS_SET_t* user_buffer);
