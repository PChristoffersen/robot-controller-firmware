#pragma once

#include <Arduino.h>
#include <config.h>

class Leds {
    public:
        using brightness_type = uint16;

        static constexpr brightness_type MIN_BRIGHTNESS { 0x0000 };
        static constexpr brightness_type MAX_BRIGHTNESS { 0xFFFF };

        enum Led {
            TOP_1,
            TOP_2,
            TOP_3,
            TOP_PWR,
            N_LEDS,
        };
        static_assert(N_LEDS==::LED_COUNT);
        enum class Mode {
            ON,
            OFF,
            BLINKING
        };

        Leds();

        void begin();

        void update();

        void set(Led led, Mode mode);
        void set_brightness(Led led, brightness_type brightness);

        static constexpr size_t count() { return static_cast<size_t>(N_LEDS); }

    private:
        static constexpr unsigned long UPDATE_INTERVAL { 250u };

        struct LedState {
            Mode mode;
            bool value;
            brightness_type brightness;
        };
        
        unsigned long m_last_update;
        LedState m_state[N_LEDS];

};