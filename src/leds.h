#pragma once

#include <Arduino.h>
#include <config.h>

class Leds {
    public:
        using brightness_type = uint16;

        static constexpr brightness_type MIN_BRIGHTNESS { 0x0000 };
        static constexpr brightness_type MAX_BRIGHTNESS { 0xFFFF };

        enum Led {
            LED_1,
            LED_2,
            LED_3,
            LED_4,
            N_LEDS,
        };
        static_assert(N_LEDS==::LED_COUNT);

        Leds();

        void begin();

        void set(Led led, brightness_type brightness);

        static constexpr size_t count() { return static_cast<size_t>(N_LEDS); }

};