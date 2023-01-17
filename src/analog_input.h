#pragma once

#include <Arduino.h>
#include <libmaple/timer.h>


class AnalogInput {
    public:
        using axis_value = int16;
        using raw_value = uint16;

        static constexpr size_t AXIS_COUNT = 6;

        AnalogInput();

        void begin();

        void tick();

        axis_value get(size_t idx) const;
        inline uint32 data_count() const
        {
            noInterrupts();
            auto val = m_adc_date_cnt;
            interrupts();
            return val;
        }
        raw_value   get_raw(size_t idx) const { return m_adc_data[idx]; }

        constexpr size_t count() const { return AXIS_COUNT; }


        inline void _isr();
    private:
        static constexpr uint32_t ADC_TIMER_INTERVAL_US { 1000000u };
        static constexpr axis_value AXIS_DEADZONE { 30u };
        static constexpr axis_value AXIS_CENTER { 2048 };
        static constexpr int32 AXIS_SCALE { 16 };

        uint32 m_adc_date_cnt; 
        uint16 m_adc_data[AXIS_COUNT];
};
