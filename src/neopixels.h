#pragma once

#include <Arduino.h>
#include <config.h>

class Neopixels {
    public:
        using color_type = uint32;

        static constexpr size_t PIXEL_COUNT { NEOPIXEL_COUNT };
        static constexpr size_t BYTES_PER_PIXEL { NEOPIXEL_BYTES_PER_PIXEL };

        static_assert(PIXEL_COUNT==::NEOPIXEL_COUNT);

        Neopixels();

        void begin();


        void show();

        void set(size_t pos, uint8 r, uint8 g, uint8 b);

        static constexpr size_t count() { return PIXEL_COUNT; }

    private:

        uint8 m_pixel_buffer[PIXEL_COUNT*BYTES_PER_PIXEL];

        static constexpr color_type from_rgb(uint8 r, uint8 g, uint8 b)
        {
            return (static_cast<color_type>(g)<<24)
                | (static_cast<color_type>(r)<<16)
                | (static_cast<color_type>(b)<<8);
        }

        inline void fill_buffer();

};
