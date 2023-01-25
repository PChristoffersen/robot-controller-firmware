#include "neopixels.h"

#include <Arduino.h>
#include <chrono>
#include <libmaple/timer.h>
#include <libmaple/dma.h>
#include <libmaple/gpio.h>


using namespace std::literals;


/* -------------------------------------
 * Hardware
*/
static constexpr uint8_t WS2812_PIN           { PA15 };
static constexpr auto    WS2812_DMA_SRC       { DMA_REQ_SRC_TIM2_CH1 };
static constexpr uint8_t WS2812_TIMER         { 2u };
static constexpr auto    WS2812_DMA_CH        { DMA_CH5 };

static timer_dev *ws2812_timer = TIMER2;
static int ws2812_timer_channel = TIMER_CH1;
static dma_dev *ws2812_dma = DMA1;
static dma_tube_config ws2812_dma_config;



/* -------------------------------------
 * Timings
*/
static constexpr uint16 WS2812_TIMER_PRESCALE { 1u };
using TimerTicks = chrono::duration<uint32_t , std::ratio<1u, F_CPU/WS2812_TIMER_PRESCALE>>;

static constexpr TimerTicks ONE_SECOND_TICKS { 1s };

static constexpr uint32_t            WS2812_CLK_FREQ           { 800000ul };
static constexpr chrono::nanoseconds WS2812_ZERO_HIGH_TIME_NS  { 400ns };
static constexpr chrono::nanoseconds WS2812_ONE_HIGH_TIME_NS   { 800ns };

static constexpr chrono::nanoseconds WS2812_RESET_PERIOD_NS    { 50us };
static constexpr chrono::microseconds WS2812_RESET_PERIOD_US   { chrono::duration_cast<chrono::microseconds>(WS2812_RESET_PERIOD_NS) };

static constexpr uint32 AUTO_RELOAD_REGISTER { (ONE_SECOND_TICKS / WS2812_CLK_FREQ).count() };
static constexpr uint32 ZERO_PULSE           { chrono::duration_cast<TimerTicks>(WS2812_ZERO_HIGH_TIME_NS).count() };
static constexpr uint32 ONE_PULSE            { chrono::duration_cast<TimerTicks>(WS2812_ONE_HIGH_TIME_NS).count() };
static constexpr uint32 RESET_TICKS          { chrono::duration_cast<TimerTicks>(WS2812_RESET_PERIOD_NS).count() };
static constexpr uint32 RESET_CYCLE_COUNT    { (RESET_TICKS / (ONE_SECOND_TICKS / WS2812_CLK_FREQ).count()) };
static constexpr uint32 HEAD_COUNT { 1 };
static constexpr uint32 TAIL_COUNT { 1 };


/// typical values for SMD5050 LEDs
static constexpr uint16 COLOR_CORRECTION_R { 0xFF };
static constexpr uint16 COLOR_CORRECTION_G { 0xB0 };
static constexpr uint16 COLOR_CORRECTION_B { 0xF0 };


/* -------------------------------------
 * Strip spec
*/
static constexpr size_t STRIP_BITS { 8*Neopixels::BYTES_PER_PIXEL * Neopixels::count() };

static volatile bool dma_running { false };
static volatile uint32 dma_end_time;
static uint16_t dma_buffer[HEAD_COUNT+STRIP_BITS+TAIL_COUNT];
static constexpr size_t DMA_BUFFER_SIZE { sizeof(dma_buffer)/sizeof(dma_buffer[0]) };


/* -------------------------------------
 * Main code
*/

static void ws2812_dma_isr()
{
    dma_end_time = micros();
    dma_running = false;
}



Neopixels::Neopixels() :
    m_dirty { false }
{

}


void Neopixels::set(size_t pos, uint8 r, uint8 g, uint8 b)
{
    #if 0
    m_pixel_buffer[pos*BYTES_PER_PIXEL]   = (COLOR_CORRECTION_G * g)/0xFF;
    m_pixel_buffer[pos*BYTES_PER_PIXEL+1] = (COLOR_CORRECTION_R * r)/0xFF;
    m_pixel_buffer[pos*BYTES_PER_PIXEL+2] = (COLOR_CORRECTION_B * b)/0xFF;
    #else
    size_t idx = pos*BYTES_PER_PIXEL;
    m_dirty |= (m_pixel_buffer[idx]!=g) || (m_pixel_buffer[idx+1]!=r) || m_pixel_buffer[idx+2]!=b;
    m_pixel_buffer[idx]   = g;
    m_pixel_buffer[idx+1] = r;
    m_pixel_buffer[idx+2] = b;
    #endif
}


void Neopixels::begin()
{
    dma_disable(DMA1, WS2812_DMA_CH);

    // Fill buffer with zeroes
    memset(dma_buffer, 0x00, sizeof(dma_buffer));
    memset(m_pixel_buffer, 0x00, sizeof(m_pixel_buffer));
    fill_buffer();

    pinMode(WS2812_PIN, OUTPUT);
    digitalWrite(WS2812_PIN, LOW);

    pinMode(WS2812_PIN, PWM);

    timer_pause(ws2812_timer);
    timer_set_mode(ws2812_timer, ws2812_timer_channel, TIMER_PWM);
    timer_set_reload(ws2812_timer, AUTO_RELOAD_REGISTER);
    timer_set_prescaler(ws2812_timer, WS2812_TIMER_PRESCALE - 1);
    timer_set_compare(ws2812_timer, ws2812_timer_channel, 0);
    timer_set_count(ws2812_timer, 0);

    timer_dma_set_base_addr(ws2812_timer, TIMER_DMA_BASE_CCR1);
    timer_dma_set_burst_len(ws2812_timer, 1);
    timer_dma_enable_req(ws2812_timer, ws2812_timer_channel);

    timer_generate_update(ws2812_timer);
    timer_resume(ws2812_timer);

    dma_disable(ws2812_dma, WS2812_DMA_CH);
    ws2812_dma_config.tube_src = dma_buffer;
    ws2812_dma_config.tube_src_size = DMA_SIZE_16BITS;
    ws2812_dma_config.tube_dst = &(ws2812_timer->regs.gen->DMAR);
    ws2812_dma_config.tube_dst_size = DMA_SIZE_16BITS;
    ws2812_dma_config.tube_nr_xfers = DMA_BUFFER_SIZE;
    ws2812_dma_config.tube_flags = DMA_CFG_SRC_INC | DMA_CFG_CMPLT_IE;
    ws2812_dma_config.target_data = nullptr;
    ws2812_dma_config.tube_req_src = WS2812_DMA_SRC;
    dma_tube_cfg(ws2812_dma, WS2812_DMA_CH, &ws2812_dma_config);
    dma_set_priority(ws2812_dma, WS2812_DMA_CH, DMA_PRIORITY_VERY_HIGH);
    dma_attach_interrupt(ws2812_dma, WS2812_DMA_CH, ws2812_dma_isr);

    show();
    show();
}




inline void Neopixels::fill_buffer()
{
    size_t idx = HEAD_COUNT;
    for (size_t i=0; i<sizeof(m_pixel_buffer); i++) { 
        auto value = m_pixel_buffer[i];
        dma_buffer[idx++] = (value&0b10000000) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b01000000) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b00100000) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b00010000) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b00001000) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b00000100) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b00000010) ? ONE_PULSE : ZERO_PULSE;
        dma_buffer[idx++] = (value&0b00000001) ? ONE_PULSE : ZERO_PULSE;
    }
}


void Neopixels::show()
{
    while (dma_running) {
        delay_us(10);
    }
    while ((micros()-dma_end_time) < WS2812_RESET_PERIOD_US.count()) {
        delay_us(10);
    }

    dma_disable(ws2812_dma, WS2812_DMA_CH);

    fill_buffer();

    dma_set_mem_addr(ws2812_dma, WS2812_DMA_CH, dma_buffer);
    dma_set_num_transfers(ws2812_dma, WS2812_DMA_CH, DMA_BUFFER_SIZE);
    dma_running = true;
    dma_enable(ws2812_dma, WS2812_DMA_CH);
    m_dirty = false;
}


