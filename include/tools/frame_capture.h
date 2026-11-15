#pragma once

#include "led_config.h"

/// Measuring tool: records the colors of one mushroom group for a number of frames, exactly as
/// they are handed to FastLED.show(), so flicker can be analysed off the device.
///
///   GET /debug/capture/start?group=2&frames=300   start (group 0..4 as in LED_MUSHROOMS_SET_t)
///   GET /debug/capture/status                     {"state": "idle|recording|done", ...}
///   GET /debug/capture/data                       binary: per frame a 4-byte micros() stamp
///                                                 (little endian), then R, G, B of each LED
void frame_capture_init();

/// Called by the LED loop once per frame, right before FastLED.show().
void frame_capture_frame(const LED_MUSHROOMS_SET_t& leds);
