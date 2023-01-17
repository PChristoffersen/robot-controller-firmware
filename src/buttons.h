#pragma once

#include <Arduino.h>

class Buttons {
    public:
        enum Switch {
            RIGHT_X,
            RIGHT_Y,
            RIGHT_A,
            RIGHT_B,
            TOP1,
            TOP2,
            TOP_PWR,
            UND,
            N_SWITCHES,
        };

        Buttons();

        void begin();

        bool get(Switch sw);

        static constexpr size_t count() { return static_cast<size_t>(N_SWITCHES); }

    private:
        struct State {
            bool value;
            uint32 last_change;
        };

        uint8_t m_data;
        State m_states[N_SWITCHES];

        inline uint8 read();
};
