#include "tools/loop_timer.h"

#include "main.h"

static const uint32_t CYCLE_US = 1000000 / FREQ_LOOP_CYCLE_HZ;
static const uint32_t REPORT_LIMIT_US = CYCLE_US * FREQ_REPORT_UPPER_LIMIT;
static const uint32_t REPORT_INTERVAL_MS = 10 * 60 * 1000; // keeps the log readable

static uint32_t cycle_start_us = 0;
static uint32_t cycles = 0;
static uint32_t freq_window_start_ms = 0;
static uint32_t overruns = 0;
static uint32_t longest_us = 0;
static uint32_t last_report_ms = 0;

void loop_timer_start_cycle() {
    cycle_start_us = micros();
}

float loop_timer_get_loop_freq() {
    const uint32_t now = millis();
    const float loop_time_ms = cycles > 0 ? (float)(now - freq_window_start_ms) / (float)cycles : 0;
    cycles = 0;
    freq_window_start_ms = now;
    return loop_time_ms > 0 ? 1000.f / loop_time_ms : 0;
}

void loop_timer_end_cycle() {
    cycles++;
    // unsigned 32-bit arithmetic stays correct when micros() wraps around
    const uint32_t elapsed_us = micros() - cycle_start_us;
    if (elapsed_us < CYCLE_US) {
        delayMicroseconds(CYCLE_US - elapsed_us);
    } else if (elapsed_us > REPORT_LIMIT_US) {
        overruns++;
        if (elapsed_us > longest_us) {
            longest_us = elapsed_us;
        }
    }

    const uint32_t now = millis();
    if (overruns > 0 && now - last_report_ms >= REPORT_INTERVAL_MS) {
        ProjectUtils.log().warn("%lu LED frames took longer than %lu ms, longest %lu ms",
                                (unsigned long)overruns, (unsigned long)(CYCLE_US / 1000),
                                (unsigned long)(longest_us / 1000));
        overruns = 0;
        longest_us = 0;
        last_report_ms = now;
    }
}
