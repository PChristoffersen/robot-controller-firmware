#pragma once

#include <Arduino.h>
#include "feature_types.h"
#include "leds.h"
#include "neopixels.h"

namespace Feature {

    class Engine {
        public:
            Engine(Leds &leds, Neopixels &neopixels);

            void begin();

            void update();

            void set_ano(uint8 input) { set_inputs(static_cast<uint32>(input)<<INPUT_BUTTON_FIRST, INPUT_BUTTON_MASK); }
            void set_usb_leds(uint8 input)   { set_inputs(static_cast<uint32>(input)<<INPUT_USB_LED_FIRST, INPUT_USB_LED_MASK); }
            void set_buttons(uint8 input) { set_inputs(static_cast<uint32>(input)<<INPUT_BUTTON_FIRST, INPUT_BUTTON_MASK); }


            bool is_dirty() const { return m_state_dirty || m_output_configs_dirty || m_mode_configs_dirty || m_color_lut_dirty || m_brightness_lut_dirty; }

            State &state() { return m_state; }
            void state_dirty() { m_state_dirty = true; }

            OutputConfigs &output_configs() { return m_output_configs; }
            void dirty_output_configs() { m_output_configs_dirty = true; }

            ModeConfigs &mode_configs() { return m_mode_configs; }
            void dirty_mode_configs() { m_mode_configs_dirty = true; }

            ColorLUT &color_lut() { return m_color_lut; }
            void dirty_color_lut() { m_color_lut_dirty = true; }

            BrightnessLUT &brightness_lut() { return m_brightness_lut; }
            void dirty_brightness_lut() { m_brightness_lut_dirty = true; }

        private:
            static constexpr uint32 UPDATE_INTERVAL { 20 };

            struct OutputState {
                uint32 start_time;
                uint8 enabled_cfg;
            };
            OutputState m_outputs[OUTPUT_COUNT];

            uint32 m_inputs;

            Leds &m_leds;
            Neopixels &m_neopixels;

            State m_state;
            bool m_state_dirty;
            uint32 m_last_update;

            OutputConfigs m_output_configs;
            bool m_output_configs_dirty;

            ModeConfigs m_mode_configs;
            bool m_mode_configs_dirty;

            ColorLUT m_color_lut;
            bool m_color_lut_dirty;

            BrightnessLUT m_brightness_lut;
            bool m_brightness_lut_dirty;

            void set_inputs(uint32 values, uint32 mask);

            void calculate_output();
    };

}