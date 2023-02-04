#include "feature_engine.h"
#include <limits>
#include <debug.h>
#include "util.h"

namespace Feature {

static const OutputConfigEntry DEFAULT_CONFIG;



Engine::Engine(Leds &leds, Neopixels &neopixels, ExternalIO &external_io):
    m_leds { leds },
    m_neopixels { neopixels },
    m_external_io { external_io },
    m_animating { false },
    m_inputs { 1<<INPUT_TRUE },
    m_input_dirty { false }
{
    for (auto &output : m_outputs) {
        output.active = false;
        output.start_time = millis();
        output.config = &DEFAULT_CONFIG;
    }
}


void Engine::begin(ConfigStore &store)
{
    m_last_update = millis();

    default_output_configs();
    default_mode_configs();
    default_color_lut();
    default_brightness_lut();
    
    dirty_soft_input();
    dirty_mode_configs();
    dirty_output_configs();
    dirty_brightness_lut();
}



void Engine::default_output_configs()
{
    for (auto &config : m_output_configs) {
        for (auto entry : config) {
            entry = OutputConfigEntry();
        }
    }
    m_output_configs_dirty = true;
}

void Engine::default_mode_configs()
{
    for (auto &config : m_mode_configs) {
        config = ModeConfig();
    }
    m_mode_configs_dirty = true;
}

void Engine::default_color_lut()
{
    for (auto &color : m_color_lut) {
        color = { 0x00, 0x00, 0x00 };
    }
    m_color_lut_dirty = true;
}

void Engine::default_brightness_lut()
{
    for (auto &brightness : m_brightness_lut) {
        brightness = 0x0000;
    }
    m_brightness_lut_dirty = true;
}





bool Engine::load_output_configs(ConfigStore &store)
{
    m_output_configs_dirty = true;
    if (!store.load(output_configs())) {
        default_output_configs();
        return false;
    }
    return true;
}

bool Engine::load_mode_configs(ConfigStore &store)
{
    m_mode_configs_dirty = true;
    if (!store.load(mode_configs())) {
        default_mode_configs();
        return false;
    }
    return true;
}

bool Engine::load_color_lut(ConfigStore &store)
{
    m_color_lut_dirty = true;
    if (!store.load(color_lut())) {
        default_color_lut();
        return false;
    }
    return true;
}

bool Engine::load_brightness_lut(ConfigStore &store)
{
    m_brightness_lut_dirty = true;
    if (!store.load(brightness_lut())) {
        default_brightness_lut();
        return false;
    }
    return true;
}


void Engine::set_inputs(uint32 values, uint32 mask)
{
    input_type value = (m_inputs & ~mask) | (values & mask);
    m_input_dirty = (value != m_inputs);
    m_inputs = value;
}


inline Engine::brightness_type Engine::ramp_brightness(uint32 now, uint32 start, const ModeConfig &config, brightness_type primary, brightness_type secondary)
{
    now = now % config.cycle_time();
    if (now < config.rise_time) {
        int32 val = static_cast<int32>(primary) - static_cast<int32>(secondary);
        val = val * static_cast<int32>(now) / config.rise_time;
        return secondary + val;
    }
    now -= config.rise_time;

    if (now < config.active_time) {
        return primary;
    }
    now -= config.active_time;

    if (now < config.fall_time) {
        int32 val = static_cast<int32>(secondary) - static_cast<int32>(primary);
        val = val * static_cast<int32>(now) / config.fall_time;
        return primary + val;
    }

    return secondary;
}


inline Engine::color_type Engine::ramp_color(uint32 now, uint32 start, const ModeConfig &config, color_type primary, color_type secondary)
{
    now = now % config.cycle_time();
    if (now < config.rise_time) {
        int32 r = static_cast<int32>(primary.red) - static_cast<int32>(secondary.red);
        int32 g = static_cast<int32>(primary.green) - static_cast<int32>(secondary.green);
        int32 b = static_cast<int32>(primary.blue) - static_cast<int32>(secondary.blue);
        r = secondary.red + r * static_cast<int32>(now) / config.rise_time;
        g = secondary.green + g * static_cast<int32>(now) / config.rise_time;
        b = secondary.blue + b * static_cast<int32>(now) / config.rise_time;
        return { static_cast<uint8>(r), static_cast<uint8>(g), static_cast<uint8>(b) };
    }
    now -= config.rise_time;

    if (now < config.active_time) {
        return primary;
    }
    now -= config.active_time;

    if (now < config.fall_time) {
        int32 r = static_cast<int32>(secondary.red) - static_cast<int32>(primary.red);
        int32 g = static_cast<int32>(secondary.green) - static_cast<int32>(primary.green);
        int32 b = static_cast<int32>(secondary.blue) - static_cast<int32>(primary.blue);
        r = primary.red + r * static_cast<int32>(now) / config.fall_time;
        g = primary.green + g * static_cast<int32>(now) / config.fall_time;
        b = primary.blue + b * static_cast<int32>(now) / config.fall_time;
        return { static_cast<uint8>(r), static_cast<uint8>(g), static_cast<uint8>(b) };
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



bool Engine::calculate_modes(uint32 now)
{
    m_animating = false;
    for (const auto &entry : m_outputs) {
        const auto &config = *entry.config;
        const auto &mode = m_mode_configs[entry.active?config.active_mode:config.passive_mode];
        m_animating |= mode.is_animated();
    }
    return m_animating;
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
        auto &config = *entry.config;
        auto &mode = m_mode_configs[entry.active?config.active_mode:config.passive_mode];
        auto brightness = ramp_brightness(now, entry.start_time, mode, m_brightness_lut[config.primary_lut], m_brightness_lut[config.secondary_lut]);
        m_external_io.put(static_cast<ExternalIO::Output>(i), brightness>0);
    }
}


#define TIME_UPDATE 0

void Engine::update()
{
    auto now = millis();

    if ( (now-m_last_update) > UPDATE_INTERVAL ) {
        bool dirty = false;

        #if TIME_UPDATE
        auto start = micros();
        #endif

        if (m_input_dirty || m_output_configs_dirty) {
            dirty |= calculate_output(now);
        }
        if (dirty || m_mode_configs_dirty) {
            dirty |= calculate_modes(now);
        }
        dirty |= m_animating;

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
        m_soft_input_dirty = false;
        m_last_update = now;

        #if TIME_UPDATE
        auto end = micros();
        Debug.print("Feature Update ");
        Debug.print(m_inputs, BIN);
        Debug.print(" ");
        Debug.print(m_animating?"A":"_");
        Debug.print(" ");
        Serial1.print((end-start)); Serial1.println(" us");
        #endif
    }

}





}
