#pragma once

#include <Arduino.h>
#include <RotaryEncoder.h>

class AnoDial {
    public:
        enum Switch {
            SW_1,
            SW_2,
            SW_3,
            SW_4,
            SW_5,
            N_SWITCHES,
        };
        static constexpr Switch SW_CENTER { Switch::SW_1 };
        static constexpr Switch SW_UP     { Switch::SW_2 };
        static constexpr Switch SW_RIGHT  { Switch::SW_3 };
        static constexpr Switch SW_DOWN   { Switch::SW_4 };
        static constexpr Switch SW_LEFT   { Switch::SW_5 };

        AnoDial();
    
        void begin();

        int get_pos();

        bool get_switch(Switch sw);

        static constexpr size_t switch_count() { return static_cast<size_t>(N_SWITCHES); }
    private:
        struct switch_state_t {
            bool value;
            unsigned long last_change;
        };
        
        switch_state_t m_states[N_SWITCHES];

};
