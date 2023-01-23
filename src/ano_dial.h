#pragma once

#include <Arduino.h>
#include <RotaryEncoder.h>

class AnoDial {
    public:
        enum Switch {
            SW_CENTER,
            SW_LEFT,
            SW_UP,
            SW_RIGHT,
            SW_DOWN,
            N_SWITCHES,
        };

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
