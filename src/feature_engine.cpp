#include "feature_engine.h"

namespace Feature {


Engine::Engine(Leds &leds, Neopixels &neopixels):
    m_leds { leds },
    m_neopixels { neopixels }
{

}


void Engine::begin()
{
    m_last_update = millis();
}


void Engine::set_inputs(uint32 values, uint32 mask)
{

}



void Engine::calculate_output()
{
    uint8 changed = 0;
    for (size_t out=0; out<OUTPUT_COUNT; out++) {
        auto &state = m_outputs[out];
        for (size_t cfg=0; cfg<OUTPUT_CONFIG_ENTRIES_PER_OUTPUT; cfg++) {
            auto &config = m_output_configs[out][cfg];
            bool enabled = (1<<config.enable) & m_inputs;
            bool active = (1<<config.active) & m_inputs;
            if (config.enable_inv) enabled = !enabled;
            if (config.active_inv) active = !active;
            if (enabled) {
                if (state.enabled_cfg != cfg) {
                    state.enabled_cfg = cfg;
                    state.start_time = true;
                    changed |= 1<<out;
                }

                break;
            }
        }
    }
}


void Engine::update()
{
    auto now = millis();

    if ( (now-m_last_update) > UPDATE_INTERVAL ) {
        m_output_configs_dirty = false;
        m_mode_configs_dirty = false;
        m_color_lut_dirty = false;
        m_brightness_lut_dirty = false;
        m_state_dirty = false;

        // Update LEDS


        // Update Neopixel

        m_last_update = now;
    }

}





}
