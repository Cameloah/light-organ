#pragma once

//  TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
//  Colors are chosen from a palette; a few palettes are provided.
//  -Mark Kriegsman, December 2015 (how it works: see twinkle.cpp)

#include <FastLED.h>

/// \brief picks the first color palette
///
void twinkle_init();

/// \brief draws the next frame of twinkles into the given LEDs
///
void twinkle_update(CRGBSet& L);
