#pragma once

#include <Arduino.h>
#include <debug.h>
#include "feature_types.h"
#include "leds.h"
#include "neopixels.h"
#include "external_io.h"

namespace Feature {

    class Engine {
        public:
            using input_type = uint32;

            Engine(Leds &leds, Neopixels &neopixels, ExternalIO &external_io);

            void begin();

            void update();

            void set_ano(uint8 input) { set_inputs(static_cast<input_type>(input)<<INPUT_BUTTON_FIRST, INPUT_BUTTON_MASK); }
            void set_usb_leds(uint8 input)   { set_inputs(static_cast<input_type>(input)<<INPUT_USB_LED_FIRST, INPUT_USB_LED_MASK); }
            void set_buttons(uint8 input) { set_inputs(static_cast<input_type>(input)<<INPUT_BUTTON_FIRST, INPUT_BUTTON_MASK); }

            State &state() { return m_state; }
            void state_dirty() { m_state_dirty = true; set_inputs(static_cast<input_type>(m_state.soft_input)<<INPUT_SOFTWARE_FIRST, INPUT_SOFTWARE_MASK); }

            OutputConfigs &output_configs() { return m_output_configs; }
            void dirty_output_configs() { m_output_configs_dirty = true; }

            ModeConfigs &mode_configs() { return m_mode_configs; }
            void dirty_mode_configs() { m_mode_configs_dirty = true; }

            ColorLUT &color_lut() { return m_color_lut; }
            void dirty_color_lut() { m_color_lut_dirty = true; }

            BrightnessLUT &brightness_lut() { return m_brightness_lut; }
            void dirty_brightness_lut() { m_brightness_lut_dirty = true; }

        private:
            using brightness_type = Leds::brightness_type;
            using color_type = ColorLUTEntry;

            static constexpr uint32 UPDATE_INTERVAL { 20 };

            struct OutputState {
                uint32 start_time;
                bool active;
                const OutputConfigEntry *config;
            };
            OutputState m_outputs[OUTPUT_COUNT];

            Leds &m_leds;
            Neopixels &m_neopixels;
            ExternalIO &m_external_io;

            input_type m_inputs;
            bool m_input_dirty;

            uint32 m_last_update;

            State m_state;
            bool m_state_dirty;

            OutputConfigs m_output_configs;
            bool m_output_configs_dirty;

            ModeConfigs m_mode_configs;
            bool m_mode_configs_dirty;

            ColorLUT m_color_lut;
            bool m_color_lut_dirty;

            BrightnessLUT m_brightness_lut;
            bool m_brightness_lut_dirty;

            void set_inputs(uint32 values, uint32 mask);

            bool calculate_output(uint32 now);
            void update_leds(uint32 now);
            void update_neopixels(uint32 now);
            void update_external(uint32 now);

            inline brightness_type ramp_brightness(uint32 now, uint32 start, const ModeConfig &config, brightness_type primary, brightness_type secondary);
            inline color_type ramp_color(uint32 now, uint32 start, const ModeConfig &config, color_type primary, color_type secondary);
    };

}