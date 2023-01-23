#include "buttons.h"

#include <Wire.h>
#include <config.h>
#include "debug.h"

/* -------------------------------------
 * Hardware
*/
static constexpr uint32 DEBOUNCE_DELAY { 30 };
static constexpr uint8 PCF8574_ADDR { 0x38 };
TwoWire HWire(1, I2C_REMAP); // i2c1



/* -------------------------------------
 * Main code
*/


Buttons::Buttons()
{

}


void Buttons::begin()
{
    pinMode(PCF8574_INT_PIN, INPUT_PULLUP);

    HWire.begin();

    HWire.beginTransmission(PCF8574_ADDR);
    HWire.write(0xFF);
    HWire.endTransmission();

    m_data = read();

    for (size_t i=0; i<count(); i++) {
        m_states[i].value = !(m_data&(1<<i));
        m_states[i].last_change = millis();
    }
}


inline uint8 Buttons::read()
{
    HWire.requestFrom(PCF8574_ADDR, 1);
    return HWire.read();
}



bool Buttons::get(Switch sw)
{
    if (digitalRead(PCF8574_INT_PIN)==LOW) {
        m_data = read();
        #if 0
        Debug.print("i2c Read ");
        Debug.print(digitalRead(PCF8574_INT_PIN));
        Debug.print(" : ");
        Debug.print(m_data, BIN);
        Debug.println();
        #endif
    }

    bool value = !(m_data&(1<<sw));
    auto now = millis();

    // Debounce
    if (value!=m_states[sw].value) {
        if ((now-m_states[sw].last_change) > DEBOUNCE_DELAY) {
            m_states[sw].value = value;
            m_states[sw].last_change = now;
        }
    }
    return m_states[sw].value;
}
