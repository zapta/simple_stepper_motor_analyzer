
#pragma once

#include <stdint.h>

#include "tft_driver.h"

class Mk3TftDriver : public TftDriver {
 public:
  virtual void begin() override;

  virtual void render_buffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                             const uint8_t* color8_p) override;

  virtual void backlight_on() override;
};
