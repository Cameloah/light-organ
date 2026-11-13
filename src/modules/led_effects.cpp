#include "modules/led_effects.h"
#include "tools/loop_timer.h"

void led_effects_run_sparkle(LED_EFFECTS_SPARKLE_t* user_buffer, const bool* trigger) {

    CRGBSet& leds = *user_buffer->led_array;
    if (*trigger) {
        // save timestamp
        user_buffer->timer_to_spark_t0 = millis();
        // reset all sparkle states
        for (int led_index = 0; led_index < leds.size(); led_index++) {
            user_buffer->sparkle_state[led_index] = false;
        }
    }

    // during sparkle time, trigger sparkles
    if ((millis() - user_buffer->timer_to_spark_t0) > user_buffer->delay_to_spark_ms &&
        (millis() - user_buffer->timer_to_spark_t0) <
            (user_buffer->time_sparkle_duration_ms + user_buffer->delay_to_spark_ms)) {
        // now sparkle in a predefined time interval
        if (((millis() - user_buffer->timer_to_spark_t0) % user_buffer->delay_between_sparks_ms) <
            (1000 / FREQ_LOOP_CYCLE_HZ)) {
            // choose a random leD
            uint8_t n = random(leds.size());
            // apply sparkle colors
            leds[n] = CRGB::White;
            // fade progressively
            uint8_t prefade_factor =
                map(static_cast<long>(millis() - user_buffer->timer_to_spark_t0 -
                                      user_buffer->delay_to_spark_ms),
                    0, user_buffer->time_sparkle_duration_ms, 0, 255);
            leds[n].fadeToBlackBy(prefade_factor);
            // mark this led as used
            user_buffer->sparkle_state[n] = true;
        }
    }

    for (int led_index = 0; led_index < leds.size(); led_index++) {
        // search array for sparkled leds to fade to black
        if (user_buffer->sparkle_state[led_index]) {
            leds[led_index].fadeToBlackBy(user_buffer->factor_fade_to_black);
        }
    }
}

void led_effects_glow_fade(CRGBSet& user_buffer, uint8_t fade_factor_min, uint8_t fade_factor_max) {
    const float fade_fraction = static_cast<float>(fade_factor_max - fade_factor_min) /
                                static_cast<float>(user_buffer.size());
    for (int i = 0; i < user_buffer.size(); i++) {
        fade_raw(&user_buffer[i], 1,
                 static_cast<uint8_t>(static_cast<float>(fade_factor_max) -
                                      fade_fraction * static_cast<float>(i)));
    }
}

void led_effect_fill_gradient_interval(CRGBSet& led_set, uint8_t start_index, uint8_t end_index,
                                       CRGB start_color, CRGB end_color) {
    uint8_t frac = 254 / led_set.size();
    for (uint8_t i = start_index; i < end_index; i++) {
        led_set[i] = blend(start_color, end_color, (start_index + i) * frac);
    }
}
