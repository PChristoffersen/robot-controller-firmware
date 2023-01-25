#include "feature_engine.h"
#include <limits>
#include <debug.h>

namespace Feature {

static const OutputConfigEntry DEFAULT_CONFIG { false };



Engine::Engine(Leds &leds, Neopixels &neopixels, ExternalIO &external_io):
    m_leds { leds },
    m_neopixels { neopixels },
    m_external_io { external_io },
    m_inputs { 1<<INPUT_TRUE },
    m_input_dirty { false }
{
    m_brightness_lut[1] = 0xFFFF;

    m_color_lut[1] = { 0x80, 0x80, 0x80 };
    m_color_lut[2] = { 0xFF, 0x00, 0x00 };
    m_color_lut[3] = { 0x00, 0xFF, 0x00 };
    m_color_lut[4] = { 0x00, 0x00, 0xFF };

    for (auto &output : m_outputs) {
        output.active = true;
        output.start_time = millis();
        output.config = &DEFAULT_CONFIG;
    }
}


void Engine::begin()
{
    m_last_update = millis();
}


void Engine::set_inputs(uint32 values, uint32 mask)
{
    input_type value = (m_inputs & ~mask) | values;
    m_input_dirty = (value != m_inputs);
    m_inputs = value;
}


inline Engine::brightness_type Engine::ramp_brightness(uint32 now, uint32 start, const ModeConfig &config, brightness_type primary, brightness_type secondary)
{
    uint32 total = config.rise_time+config.active_time+config.fall_time+config.passive_time;
    now = now%total;
    if (now < config.rise_time) {
        // TODO
        return primary;
    }
    now -= config.rise_time;

    if (now < config.active_time) {
        return primary;
    }
    now -= config.active_time;

    if (now < config.fall_time) {
        // TODO
        return secondary;
    }

    return secondary;
}


inline Engine::color_type Engine::ramp_color(uint32 now, uint32 start, const ModeConfig &config, color_type primary, color_type secondary)
{
    uint32 total = config.rise_time+config.active_time+config.fall_time+config.passive_time;
    now = now%total;
    if (now < config.rise_time) {
        // TODO
        return primary;
    }
    now -= config.rise_time;

    if (now < config.active_time) {
        return primary;
    }
    now -= config.active_time;

    if (now < config.fall_time) {
        // TODO
        return secondary;
    }

    return secondary;
}



bool Engine::calculate_output(uint32 now)
{
    bool changed = false;
    for (size_t out=0; out<OUTPUT_COUNT; out++) {
        auto &state = m_outputs[out];
        bool found = false;
        for (size_t cfg=0; cfg<OUTPUT_CONFIG_ENTRIES_PER_OUTPUT; cfg++) {
            auto &config = m_output_configs[out][cfg];
            bool enabled = (1<<config.enable) & m_inputs;
            bool active = (1<<config.active) & m_inputs;
            if (config.enable_inv) enabled = !enabled;
            if (config.active_inv) active = !active;
            if (enabled) {
                if (state.config != &config) {
                    state.config = &config;
                    state.start_time = now;
                    changed = true;
                }
                changed |= (state.active!=active);
                state.active = active;
                found = true;
                break;
            }
        }
        if (!found && state.config!=&DEFAULT_CONFIG) {
            state.config = &DEFAULT_CONFIG;
            state.active = false;
            state.start_time = millis();
            changed = true;
        }
    }
    return changed;
}


void Engine::update_leds(uint32 now)
{
    for (size_t i=0; i<OUTPUT_LED_COUNT; i++) {
        auto &entry = m_outputs[i+OUTPUT_LED_FIRST];
        auto &config = *entry.config;
        auto &mode = m_mode_configs[entry.active?config.active_mode:config.passive_mode];
        auto brightness = ramp_brightness(now, entry.start_time, mode, m_brightness_lut[config.primary_lut], m_brightness_lut[config.secondary_lut]);
        m_leds.set(static_cast<Leds::Led>(i), brightness);
    }
}


void Engine::update_neopixels(uint32 now)
{
    for (size_t i=0; i<OUTPUT_NEOPIXEL_COUNT; i++) {
        auto &entry = m_outputs[i+OUTPUT_NEOPIXEL_FIRST];
        auto &config = *entry.config;
        auto &mode = m_mode_configs[entry.active?config.active_mode:config.passive_mode];
        auto color = ramp_color(now, entry.start_time, mode, m_color_lut[config.primary_lut], m_color_lut[config.secondary_lut]);
        m_neopixels.set(i, color.red, color.green, color.blue);
    }
}

void Engine::update_external(uint32 now)
{
    for (size_t i=0; i<OUTPUT_EXT_COUNT; i++) {
        auto &entry = m_outputs[i+OUTPUT_EXT_FIRST];
        m_external_io.put(static_cast<ExternalIO::Output>(i), entry.active);
    }
}


void Engine::update()
{
    auto now = millis();

    if ( (now-m_last_update) > UPDATE_INTERVAL ) {
        bool dirty = false;

        dirty = true;
        #if 1
        auto start = micros();
        #endif

        if (m_input_dirty || m_output_configs_dirty) {
            dirty |= calculate_output(now);
        }
        if (m_mode_configs_dirty) {
            dirty = true;
        }

        // Update Outputs
        if (dirty || m_brightness_lut_dirty) {
            update_leds(now);
        }
        if (dirty || m_color_lut_dirty) {
            update_neopixels(now);
        }
        if (dirty) {
            update_external(now);
        }


        m_mode_configs_dirty = false;
        m_color_lut_dirty = false;
        m_brightness_lut_dirty = false;
        m_output_configs_dirty = false;
        m_input_dirty = false;
        m_state_dirty = false;
        m_last_update = now;

        #if 1
        auto end = micros();
        Debug.print("Feature Update ");
        Debug.print(m_inputs, BIN);
        Debug.print(" ");
        Serial1.print((end-start)); Serial1.println(" us");
        #endif
    }

}





}
