#pragma once

#include <Arduino.h>

class ExternalIO {
    public:
        enum Input {
            IN_1,
            IN_2,
            N_INPUT
        };
        enum Output {
            OUT_1,
            OUT_2,
            N_OUTPUT
        };

        ExternalIO();

        void begin();

        bool get(Input input);
        void put(Output output, bool state);


        static constexpr size_t input_count() { return N_INPUT; }
        static constexpr size_t output_count() { return N_OUTPUT; }
    private:

};