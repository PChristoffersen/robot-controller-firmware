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

static constexpr uint16 PWM_MAX { 0xFFFF };


Leds::Leds() 
{
}


void Leds::begin()
{
    for (size_t i=0; i<N_LEDS; i++) {
        const auto pin = LED_PINS[i];
        pinMode(pin, PWM);
        pwmWrite(pin, PWM_MAX);
    }
}


void Leds::set(Led led, brightness_type brightness)
{
    pwmWrite(LED_PINS[static_cast<size_t>(led)], PWM_MAX-brightness);
}

