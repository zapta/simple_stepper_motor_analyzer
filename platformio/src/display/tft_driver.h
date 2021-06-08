
#pragma once

#include <stdint.h>

class TftDriver {
    public:

   virtual void begin() = 0;

   virtual void render_buffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                          const uint8_t* color8_p) = 0;

    virtual void backlight_on() = 0;
    virtual void hardware_test() = 0;
};
