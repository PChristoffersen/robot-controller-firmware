#include "external_io.h"

#include <config.h>

static constexpr uint8 INPUT_PINS[ExternalIO::input_count()] {
    EXT_IN1,
    EXT_IN2,
};

static constexpr uint8 OUTPUT_PINS[ExternalIO::output_count()] {
    EXT_OUT1,
    EXT_OUT2,
};



ExternalIO::ExternalIO()
{
}



void ExternalIO::begin()
{
    for (size_t i=0; i<input_count(); i++) {
        pinMode(INPUT_PINS[i], INPUT_PULLDOWN);
    }

    for (size_t i=0; i<output_count(); i++) {
        pinMode(OUTPUT_PINS[i], OUTPUT);
        digitalWrite(OUTPUT_PINS[i], LOW);
    }
}


bool ExternalIO::get(Input input)
{
    return digitalRead(INPUT_PINS[static_cast<size_t>(input)])!=LOW;
}


void ExternalIO::put(Output output, bool state)
{
    digitalWrite(OUTPUT_PINS[static_cast<size_t>(output)], state?HIGH:LOW);
}
