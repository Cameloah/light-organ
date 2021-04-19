#include "modules/music_visualisation.h"

// buffers to store max values for scaling
float _music_vis_raw_max_input_base           = 0;
float _music_vis_raw_max_input_mid            = 0;
float _music_vis_raw_max_input_treble_l       = 0;
float _music_vis_raw_max_input_treble_r       = 0;

void music_vis_update() {
    // scale the measured ampitudes to a max of 1023.
    uint16_t _music_vis_scale_limit_base = MUSIC_VIS_SCALE_LIMIT_BASE;
    uint16_t _music_vis_scale_limit_mid = MUSIC_VIS_SCALE_LIMIT_MID;
    filters_scale(spectrum_analyzer_results.amplitude_left[1] < spectrum_analyzer_results.amplitude_left[1]
                  ? spectrum_analyzer_results.amplitude_left[1] : spectrum_analyzer_results.amplitude_left[1],
                  _music_vis_raw_max_input_base, _music_vis_scale_limit_base);

    uint16_t total_volume = spectrum_total_volume();
    filters_scale(total_volume, _music_vis_raw_max_input_base, _music_vis_scale_limit_mid);

    
}

