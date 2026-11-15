#include "modules/ocean.h"

//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other.
// (The original also adds "whitecaps" where the waves line up and deepens
// the blues and greens; both are left out here.)
//
// The speed and scale and motion each layer varies slowly within independent
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
//////////////////////////////////////////////////////////////////////////

static CRGBPalette16 pacifica_palette_1 = {
    0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
    0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50};
static CRGBPalette16 pacifica_palette_2 = {
    0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
    0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
static CRGBPalette16 pacifica_palette_3 = {
    0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
    0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF};

static CRGBSet* user_buffer;

// The layers are added up in fine units and rounded only once per LED and frame. Rounding each
// layer on its own (as FastLED's 8-bit ColorFromPalette does) makes the layers step at different
// moments, often in opposite directions, so the sum jumped back and forth between neighbouring
// levels many times a second although the waves themselves change slowly: visible as shimmer in
// the dim blues.
static const int kMaxLeds = 64;
static const int32_t kFine = 4096; // fine units per color step
static int32_t sum[kMaxLeds][3];   // the frame being built, in fine units
static int ledCount = 0;

static void pacifica_loop();
static void pacifica_one_layer(const CRGBPalette16& p, uint16_t cistart, uint16_t wavescale,
                               uint8_t bri, uint16_t ioff);

void ocean_update(CRGBSet& user_leds) {
    user_buffer = &user_leds;
    ledCount = user_leds.len < kMaxLeds ? user_leds.len : kMaxLeds;
    pacifica_loop();
    for (int i = 0; i < ledCount; i++) {
        CRGB& led = user_buffer->operator[](i);
        for (int channel = 0; channel < 3; channel++) {
            const int32_t level = (sum[i][channel] + kFine / 2) / kFine; // the only rounding
            led.raw[channel] = static_cast<uint8_t>(level > 255 ? 255 : level);
        }
    }
}

static void pacifica_loop() {
    // Increment the four "color index start" counters, one for each wave layer.
    // Each is incremented at a different speed, and the speeds vary over time.
    static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    static uint32_t sLastms = 0;
    uint32_t ms = GET_MILLIS();
    uint32_t deltams = ms - sLastms;
    sLastms = ms;
    uint16_t speedfactor1 = beatsin16(3, 179, 269);
    uint16_t speedfactor2 = beatsin16(4, 179, 269);
    uint32_t deltams1 = (deltams * speedfactor1) / 256;
    uint32_t deltams2 = (deltams * speedfactor2) / 256;
    uint32_t deltams21 = (deltams1 + deltams2) / 2;
    sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
    sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
    sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
    sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

    // Start from a dim background blue-green
    for (int i = 0; i < ledCount; i++) {
        sum[i][0] = 2 * kFine;
        sum[i][1] = 6 * kFine;
        sum[i][2] = 10 * kFine;
    }

    // Render each of four layers, with different scales and speeds, that vary over time
    pacifica_one_layer(pacifica_palette_1, sCIStart1, beatsin16(3, 11 * 256, 14 * 256),
                       beatsin8(10, 70, 130), 0 - beat16(301));
    pacifica_one_layer(pacifica_palette_2, sCIStart2, beatsin16(4, 6 * 256, 9 * 256),
                       beatsin8(17, 40, 80), beat16(401));
    pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256, beatsin8(9, 10, 38),
                       0 - beat16(503));
    pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256, beatsin8(8, 10, 28), beat16(601));
}

// Add one layer of waves, in fine units. Same as FastLED's ColorFromPalette(p, index, bri,
// LINEARBLEND), but with the position between two palette entries kept to 1/4096 instead of 1/16
// and nothing rounded.
static void pacifica_one_layer(const CRGBPalette16& p, uint16_t cistart, uint16_t wavescale,
                               uint8_t bri, uint16_t ioff) {
    uint16_t ci = cistart;
    uint16_t waveangle = ioff;
    uint16_t wavescale_half = (wavescale / 2) + 20;
    for (int i = 0; i < ledCount; i++) {
        waveangle += 250;
        uint16_t s16 = sin16(waveangle) + 32768;
        uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
        ci += cs;
        const uint32_t sindex16 = static_cast<uint16_t>(sin16(ci) + 32768);
        // palette position 0..240 (as scale16(sindex16, 240)) with 12 fractional bits
        const uint32_t position = (sindex16 * 240U) >> 4U;
        const auto entry = static_cast<uint8_t>(position >> 16U); // which of the 16 entries, 0..14
        const auto between = static_cast<int32_t>((position & 0xFFFFU) >> 4U); // 0..4095
        const CRGB& from = p[entry];
        const CRGB& to = p[static_cast<uint8_t>(entry + 1)];
        for (int channel = 0; channel < 3; channel++) {
            const int32_t color = from.raw[channel] * kFine +
                                  (to.raw[channel] - from.raw[channel]) * between; // fine units
            sum[i][channel] += color * bri / 256;
        }
    }
}
