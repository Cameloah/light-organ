#pragma once

//
//  "Pacifica"
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
//

#include <FastLED.h>

/// \brief draws the next frame of the ocean waves into the given LEDs
///
void ocean_update(CRGBSet& user_leds);
