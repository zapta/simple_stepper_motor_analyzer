
#pragma once

#include <stdint.h>

class TftDriver {
 public:
  void begin();

  void render_buffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                     const uint8_t* color8_p);

  void backlight_on();
};
