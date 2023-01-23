#include "analog_input.h"
#undef abs

#include <algorithm>
#include <chrono>
#include <limits>
#include <libmaple/adc.h>
#include <libmaple/dma.h>
#include <libmaple/timer.h>

#include <config.h>
#include <debug.h>
#include "util.h"

using namespace std::literals;


/* -------------------------------------
 * Hardware
*/

static constexpr uint8_t ANALOG_INPUT_PINS[AnalogInput::AXIS_COUNT] = {
    AXIS_LEFT_X_PIN,
    AXIS_LEFT_Y_PIN,
    AXIS_LEFT_Z_PIN,
    AXIS_RIGHT_X_PIN,
    AXIS_RIGHT_Y_PIN,
    AXIS_RIGHT_Z_PIN,
};
static constexpr size_t ANALOG_INPUT_PIN_COUNT { sizeof(ANALOG_INPUT_PINS)/sizeof(ANALOG_INPUT_PINS[0]) };
static_assert(ANALOG_INPUT_PIN_COUNT==AnalogInput::AXIS_COUNT);


static adc_dev              *g_adc            { ADC1 };
static timer_dev            *g_timer          { TIMER1 };
static constexpr uint8       g_timer_channel  { TIMER_CH1 };
static dma_dev              *g_dma            { DMA1 };
static constexpr dma_channel g_dma_channel    { DMA_CH1 };
static dma_tube_config  g_dma_config;

/* -------------------------------------
 * Timings
*/
/*
    In toggle mode the ADC only triggers on every other timer interrupt.
    therefore the tick ratio numerator is 2
*/
static constexpr uint16 ANALOG_INPUT_TIMER_PRESCALE { 72/8 };
using AxisTimerTicks = chrono::duration<uint32_t , std::ratio<2, F_CPU/ANALOG_INPUT_TIMER_PRESCALE>>;

static constexpr AxisTimerTicks ANALOG_INPUT_TIMER_INTERVAL { 5ms };
static constexpr uint16 ANALOG_INPUT_TIMER_INTERVAL_COUNT { ANALOG_INPUT_TIMER_INTERVAL.count() };
static_assert(ANALOG_INPUT_TIMER_INTERVAL.count()<0xFFFF);

/* -------------------------------------
 * Utility functions
*/

void adc_dma_enable(adc_dev * dev)
{
    dev->regs->CR2 |= ADC_CR2_DMA;
}

static void adc_set_pins(adc_dev *dev, const uint8 *pins, uint8 length){
    //convert pins to channels.
    uint8 channels[length];
    unsigned int records[3] = {0,0,0};
    
    for (uint8_t i = 0; i < length; i++) { //convert the channels from pins to ch.
        channels[i] = PIN_MAP[pins[i]].adc_channel;
    }

    //run away protection
    if (length > 16) length = 16;

    //write the length
    records[2] |= (length - 1) << 20;

    //i goes through records, j goes through variables.
    for (uint8_t i = 0, j = 0; i < length; i++) {//go through the channel list.
        if (i!=0 && i%6 == 0) j++;//next variable, please!!
        records[j] |= (channels[i] << ((i%6)*5));
    }
    //update the registers inside with the scan sequence.
    dev->regs->SQR1 = records[2];
    dev->regs->SQR2 = records[1];
    dev->regs->SQR3 = records[0];
}

static void adc_set_scan_mode(adc_dev *dev)
{
	dev->regs->CR1 |= ADC_CR1_SCAN;
}

#if 0
static void adc_set_continuous(adc_dev *dev) {
	dev->regs->CR2 |= ADC_CR2_CONT;
}


static void adc_start_conversion(adc_dev *dev) {
    dev->regs->CR2 |= ADC_CR2_SWSTART;
}

static void timer_set_master_mode(const timer_dev *dev, uint32 mode)
{
	dev->regs.bas->CR2 &= ~TIMER_CR2_MMS;
	dev->regs.bas->CR2 |= mode;
}

#endif





/* -------------------------------------
 * Main code
*/

#define DEBUG_INTERRUPTS 0

static AnalogInput *g_instance = nullptr;
static AnalogInput::axis_value g_dma_data[AnalogInput::AXIS_COUNT];


inline void AnalogInput::_isr()
{
    for (size_t i=0; i<AXIS_COUNT; i++) {
        auto &data = m_adc_data[i][m_adc_data_next];
        m_adc_data_total[i] -= data;
        data = g_dma_data[i];
        m_adc_data_total[i] += data;
    }
    m_adc_data_next++;
    if (m_adc_data_next>=ADC_DATA_AVG_COUNT) {
        m_adc_data_next = 0;
    }
    m_adc_data_cnt++;
}


static void analog_input_dma_isr()
{
    #if DEBUG_INTERRUPTS
    static uint32 last = 0;
    auto now = millis();
    DebugPrint(now-last);
    DebugPrintLn(" DISR");
    last = now;
    #endif

    g_instance->_isr();
}

#if DEBUG_INTERRUPTS
static void analog_input_timer_isr()
{
    static uint32 last = 0;
    auto now = millis();
    DebugPrint(now-last);
    DebugPrintLn(" AISR");
    last = now;
}
#endif


AnalogInput::AnalogInput() :
    m_adc_data_next { 0 }
{
    for (size_t i=0; i<AXIS_COUNT; i++) {
        for (size_t j=0; j<ADC_DATA_AVG_COUNT; j++) {
            m_adc_data[i][j] = AXIS_CENTER;
        }
        m_adc_data_total[i] = ADC_DATA_AVG_COUNT*AXIS_CENTER;
    }
}




void AnalogInput::begin()
{
    g_instance = this;

    // Configure pins, and fill data with initial values
    for (size_t i=0; i<ANALOG_INPUT_PIN_COUNT; i++) {
        auto pin = ANALOG_INPUT_PINS[i];
        gpio_set_mode(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit, GPIO_INPUT_ANALOG);
    }

    adc_calibrate(g_adc);
    adc_set_sample_rate(g_adc, ADC_SMPR_55_5);
    adc_set_pins(g_adc, ANALOG_INPUT_PINS, ANALOG_INPUT_PIN_COUNT);
    adc_set_scan_mode(g_adc);
    adc_set_extsel(g_adc, ADC_EXT_EV_TIM1_CC1);
    adc_dma_enable(g_adc);

    dma_disable(g_dma, g_dma_channel);
    g_dma_config.tube_src = &g_adc->regs->DR;
    g_dma_config.tube_src_size = DMA_SIZE_16BITS;
    g_dma_config.tube_dst = g_dma_data;
    g_dma_config.tube_dst_size = DMA_SIZE_16BITS;
    g_dma_config.tube_nr_xfers = AXIS_COUNT;
    g_dma_config.tube_flags = DMA_CFG_DST_INC | DMA_CIRC_MODE | DMA_CFG_CMPLT_IE;
    g_dma_config.target_data = nullptr;
    g_dma_config.tube_req_src = DMA_REQ_SRC_ADC1;
    dma_tube_cfg(g_dma, g_dma_channel, &g_dma_config);
    dma_attach_interrupt(g_dma, g_dma_channel, analog_input_dma_isr);
    dma_enable(g_dma, g_dma_channel);

    timer_pause(g_timer);
    timer_set_prescaler(g_timer, ANALOG_INPUT_TIMER_PRESCALE-1);
    timer_set_reload(g_timer, ANALOG_INPUT_TIMER_INTERVAL.count());
    timer_oc_set_mode(g_timer, g_timer_channel, TIMER_OC_MODE_TOGGLE, 0);
    timer_cc_enable(g_timer, g_timer_channel);
    timer_set_compare(g_timer, g_timer_channel, 1);
    #if DEBUG_INTERRUPTS
    timer_attach_interrupt(g_timer, TIMER_CC1_INTERRUPT, analog_input_timer_isr);
    #endif
    timer_generate_update(g_timer);
    timer_resume(g_timer);

}


AnalogInput::axis_value AnalogInput::get(size_t idx) const
{
    noInterrupts();
    auto data = m_adc_data_total[idx];
    interrupts();
    int32 val = static_cast<int32>(data/ADC_DATA_AVG_COUNT)-AXIS_CENTER;
    val = std::abs(val) < AXIS_DEADZONE ? 0 : val;
    return clamp_value<int32>(val*std::numeric_limits<int16>::max()/AXIS_MAX, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max());
}


