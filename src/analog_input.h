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

        axis_value get(size_t idx) const;
        inline uint32 data_count() const
        {
            noInterrupts();
            auto val = m_adc_data_cnt;
            interrupts();
            return val;
        }
        raw_value   get_raw(size_t idx) const { return m_adc_data_total[idx]/ADC_DATA_AVG_COUNT; }

        constexpr size_t count() const { return AXIS_COUNT; }


        inline void _isr();
    private:
        static constexpr size_t ADC_DATA_AVG_COUNT { 4 };
        static constexpr axis_value AXIS_CENTER { 2048 };
        static constexpr axis_value AXIS_MAX { 2048 };

        uint32 m_adc_data_cnt; 
        uint16 m_adc_data[AXIS_COUNT][ADC_DATA_AVG_COUNT];
        uint32 m_adc_data_next;
        volatile uint32 m_adc_data_total[AXIS_COUNT];
};
