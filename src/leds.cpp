#include "leds.h"
#include <config.h>

/* -------------------------------------
 * Hardware
*/

static constexpr uint8 LED_PINS[Leds::count()] {
    LED1_PIN,
    LED2_PIN,
    LED3_PIN,
    LED4_PIN,
};

static constexpr uint16 LED_PWM_MIN { 0 };
static constexpr uint16 LED_PWM_MAX { 0xFFFF };


Leds::Leds() 
{
    for (auto &state : m_state) {
        state.value = false;
        state.mode = Mode::OFF;
    }

}


void Leds::begin()
{
    for (size_t i=1; i<N_LEDS; i++) {
        const auto pin = LED_PINS[i];
        pinMode(pin, PWM);
        pwmWrite(pin, LED_PWM_MIN);
        m_state[i].value = false;
        m_state[i].mode = Mode::OFF;
        m_state[i].brightness = MAX_BRIGHTNESS;
    }
    m_state[TOP_PWR].mode = Mode::BLINKING;
}


void Leds::set(Led led, Mode mode)
{
    auto &state = m_state[static_cast<size_t>(led)];
    if (state.mode==mode)
        return;

    const auto pin = LED_PINS[static_cast<size_t>(led)];

    state.mode = mode;
    switch (state.mode) {
        case Mode::ON:
            pwmWrite(pin, LED_PWM_MAX);
            state.value = true;
            break;
        case Mode::OFF:
            pwmWrite(pin, LED_PWM_MIN);
            state.value = false;
            break;
        case Mode::BLINKING:
            break;
    }
}


void Leds::set_brightness(Led led, brightness_type brightness)
{
    m_state[static_cast<size_t>(led)].brightness = brightness;
}


void Leds::update()
{
    auto now = millis();

    if ((now-m_last_update)>UPDATE_INTERVAL) {
        for (size_t i=0; i<N_LEDS; i++) {
            auto &state = m_state[i];
            const auto pin = LED_PINS[i];
            if (state.mode==Mode::BLINKING) {
                state.value = !state.value;
                pwmWrite(pin, state.value?state.brightness:MIN_BRIGHTNESS);
            }
        }
        m_last_update = now;
    }
}
