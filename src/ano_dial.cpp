#include "ano_dial.h"

#include <RotaryEncoder.h>
#include <config.h>

/* -------------------------------------
 * Hardware
*/
static constexpr uint8 ANO_SWITCH_PINS[AnoDial::switch_count()] = {
    ANO_SW1_PIN, ANO_SW2_PIN, ANO_SW3_PIN, ANO_SW4_PIN, ANO_SW5_PIN, 
}; 
static RotaryEncoder *ano_encoder = nullptr;
static constexpr uint32 ANO_DEBOUNCE_DELAY { 30 };



/* -------------------------------------
 * Main code
*/


static void ano_pin_isr()
{
    ano_encoder->tick();
}




AnoDial::AnoDial()
{

}


void AnoDial::begin()
{
    // Setup switches
    for (size_t i=0; i<switch_count(); i++) {
        pinMode(ANO_SWITCH_PINS[i], INPUT_PULLUP);
        m_states[i].value = digitalRead(ANO_SWITCH_PINS[i])==LOW;
        m_states[i].last_change = millis();
    }

    // Setup encoder
    pinMode(ANO_ENCA_PIN, INPUT_PULLUP);
    pinMode(ANO_ENCB_PIN, INPUT_PULLUP);
    ano_encoder = new RotaryEncoder(ANO_ENCA_PIN, ANO_ENCB_PIN, RotaryEncoder::LatchMode::TWO03);
    attachInterrupt(digitalPinToInterrupt(ANO_ENCA_PIN), ano_pin_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ANO_ENCB_PIN), ano_pin_isr, CHANGE);
}


int AnoDial::get_pos()
{
    int pos; 
    noInterrupts();
    pos = ano_encoder->getPosition();
    interrupts();
    return pos;
}




bool AnoDial::get_switch(Switch sw)
{
    bool value = digitalRead(ANO_SWITCH_PINS[sw])==LOW;
    auto now = millis();

    // Debounce
    if (value!=m_states[sw].value) {
        if ((now-m_states[sw].last_change) > ANO_DEBOUNCE_DELAY) {
            m_states[sw].value = value;
            m_states[sw].last_change = now;
        }
    }
    return m_states[sw].value;
}
