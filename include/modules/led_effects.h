#include <FastLED.h>

#define LED_EFFECTS_SPARKLE_STATE_ARR_SIZE                  32

typedef struct
{
    CRGBSet* led_array;
    uint16_t delay_to_spark_ms;
    uint16_t delay_between_sparks_ms;
    uint64_t timer_to_spark_t0;
    uint16_t time_sparkle_duration_ms;
    uint8_t factor_fade_to_black;
    bool sparkle_state[LED_EFFECTS_SPARKLE_STATE_ARR_SIZE];
} LED_EFFECTS_SPARKLE_t;


/// \brief creates a ramdomly generated sparkle effect after a predefined delay.
/// all the necessary data is being stored in the user buffer.
/// Upon a trigger detection first a timestamp is marked and all leds are marked
/// as ready to sparkle.
/// Next, after the delay_to_spark_ms, WHITE is applied to a random led each loop
/// iteration, for an entire duration of time_sparkle_duration_ms. The led is then
/// marked and faded rapidly back to black in a last step.
///
/// \param user_buffer contains the buffers for the sparkle effect as well as the
/// array of the leds
/// \param trigger pointer to the trigger flag
///
void led_effects_run_sparkle(LED_EFFECTS_SPARKLE_t* user_buffer, bool* trigger);

void led_effects_glow_fade(CRGBSet& user_buffer, uint8_t fade_factor_min, uint8_t fade_factor_max);

void testfct(CRGBSet& user_buffer);